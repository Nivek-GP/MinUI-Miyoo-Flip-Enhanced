# Audio, the frame budget, and how they are coupled

Notes from tracking down PS1 audio crackling on the Miyoo Flip (my355). The
useful lesson was not about audio at all: **most "audio" problems in this
frontend are frame-timing problems**, and they are diagnosable with numbers
instead of guesswork.

Read this before changing anything in `SND_*` (`workspace/all/common/api.c`) or
the main loop in `workspace/all/minarch/minarch.c`.

## How the audio path works

The core pushes samples synchronously from inside `core.run()` — the frontend
never pulls, so **the core alone decides when audio arrives**. Those samples go
through `SND_batchSamples` into a ring buffer, and SDL's audio thread drains it
in the background.

Three numbers define the ring (`api.c`, set in `SND_init`):

| Constant | Meaning |
|---|---|
| `SND_RING_FRAMES_MULT` (8) | ring capacity, ~133ms — headroom, not latency |
| `SND_TARGET_FRAMES_MULT` (3) | the setpoint, ~50ms — **this is what sets latency** |
| `SND_RATIO_TRIM` (0.01) | how far playback rate may be nudged: **±1%** |

A proportional controller nudges playback rate to hold the fill at the setpoint.
The ring is primed to the setpoint at init so latency starts where it belongs
instead of climbing from empty.

Two failure modes, and they look identical from the couch:

- **Overflow** — the core produces faster than realtime. The ring fills and
  `SND_batchSamples` drops the tail. Each discard is a waveform discontinuity.
- **Underrun** — the core produces slower than realtime. The ring empties and
  `SND_audioCallback` zero-fills. Each gap is a discontinuity too.

Both sound like crackling. The counters tell them apart instantly; ears cannot.

### The ±1% rule, which explains almost everything

The controller can only stretch or compress playback by 1%. So:

> **If the core cannot hold within ~1% of realtime, the audio will break up, and
> no audio setting can save it.**

At 60fps target, that means anything below roughly **59.4fps sustained** starves
the ring permanently — it can never refill, because refilling requires the core
to run *faster* than realtime, which it never does. Occupancy sits near zero and
every dip is a dropout.

This is why chasing audio settings for a crackling problem is usually wasted
effort. Check the frame rate first.

## The frame budget

Each frame must fit in `1000 / core.fps` ms — 16.67ms at 60fps. Two things
consume it:

```
emulation  +  present (GFX_flip)  <=  16.67ms
```

`GFX_flip` happens *inside* the core's video callback, so it is counted inside
`core.run()`. When profiling, **emulation cost is `run - flip`**.

Measured on my355, Bloody Roar II gameplay:

| | emulation | flip | total | result |
|---|---|---|---|---|
| Crisp | 10.2ms | **8.5ms** | 18.7ms | 53fps, ~40 underruns/sec |
| Sharp | 10.2ms | **5.0ms** | 15.2ms | 60.0fps, zero underruns |

`SHARPNESS_CRISP` does an extra render-to-texture pass in `PLAT_flip`
(`workspace/my355/platform/platform.c`). That pass costs ~3.5ms — over 20% of the
frame budget, paid by **every core, every frame**. It is invisible on cheap cores
(GBA emulates in 2-6ms and fits either way) and fatal on expensive ones.

The present cost is not free and not fixed. Treat it as a first-class part of the
budget, not as "just drawing".

## What actually paces the core

Nothing in minarch tells the core to slow down. It gets paced as a side effect of
blocking, and **where that block happens differs by mode** — worth knowing before
changing either loop.

**Normal (threaded video off).** `GFX_flip` is called from inside the core's own
video callback, so the core is still inside `core.run()` when the flip blocks on
vsync. That is the pacing. The fractional deadline in the main loop is only a
*backstop* for frames that skip the flip entirely.

**Prioritize Audio (threaded video on).** The flip moves to the main thread, so
the core never touches vsync. What paces it instead is the mutex: the main thread
reacquires `core_mx` when `pthread_cond_wait` returns and **holds it across the
vsync-blocking `GFX_flip`**, so the core thread's next `video_refresh_callback`
blocks on that same mutex. Measured on GBA: 60 core runs/sec, `under=0`,
`drop=0`. The option works.

**Null frames escape both.** `video_refresh_callback` opens with
`if (!data) return;`, so a duped frame never reaches the flip and never takes the
mutex. A core that emits them outruns whichever mechanism is in play:

- Non-threaded, it misses vsync — which is why the deadline backstop exists, and
  why its old whole-millisecond truncation ran cores 4% fast (`39feb41`,
  `d9f49ca`).
- Threaded, it misses the mutex and runs genuinely unpaced.

PS1 emits null frames constantly, which is why Prioritize Audio is locked off in
`PS.pak/default.cfg` and works fine everywhere else. The scope of that lock is
**null-frame cores, not platforms** — do not generalise it.

When reasoning about a pacing change, ask which of these three the core is
actually hitting. Claiming "nothing throttles this loop" without checking the
mutex is a mistake already made here once.

## Diagnosing: build with DEBUG

```
make PLATFORM=my355 DEBUG=1 -C workspace/all/minarch
```

That enables two log lines per second, written to `$LOGS_PATH/<TAG>.txt` (the pak
`launch.sh` already redirects stdout there):

```
snd:  under=N/Mfr drop=N/Mfr occ=min/avg/max of RING (target T) clamp=N/N fps=F cpu=C% clk=MODE
prof: frames=N run=X.XXms flip=Y.YYms(N) sleep=Z.ZZms total=W.WWms
```

How to read them:

| Field | Healthy | What it means when it is not |
|---|---|---|
| `under=` | `0/0fr` | ring starving — core is too slow |
| `drop=` | `0/0fr` | ring overflowing — core is too fast |
| `occ=` | hovering near `target` | pinned at 0 = starving; pinned near capacity = overflowing |
| `clamp=` | well below the batch count | saturated (`60/60`) = the ±1% controller has given up |
| `run - flip` | — | true emulation cost |
| `flip` | — | present cost, including any vsync wait |
| `sleep` | >0 | headroom exists; `0.00ms` means the loop never idles |

Note the log is **truncated on every launch** (`>` in `launch.sh`), so one game
per capture. Core option changes are logged inline with their own lines, so
several can be compared in a single session; frontend options are not, which is
why CPU mode is appended to the `snd:` line as `clk=`.

## Traps

**Integer truncation in pacing.** `(uint32_t)(1000.0/60.0)` is 16, not 16.667 —
that paced the core at 62.5fps, 4.2% fast, and overflowed the ring by ~1,800
samples/sec. The floor only takes over when a frame skips `GFX_flip` and so
misses the vsync that normally paces things, which PS1 cinematics do constantly
via null frames. Always carry the fractional millisecond, and sleep to an
absolute deadline so a late wake-up shortens the next sleep instead of
accumulating.

**Do not re-add an FPS term to the rate controller.** Tried and reverted
(`e918557`): missed vsyncs on PS1 pitch-shifted audio by up to 15%. Earlier
attempts also caused a heap overflow (`51d6aed`) and an init bug (`834c4aa`).

**Do not raise `SND_TARGET_FRAMES_MULT` globally.** It is the latency setpoint and
it is low on purpose so GBA rhythm games stay playable (`71c1b7f`). Scope any
increase to the core that needs it.

**Do not switch the resampler off `SRC_LINEAR`.** `SRC_SINC_*` was tried and
reverted for CPU cost and frame-pacing damage on this SoC family. It is also
usually moot: in and out are typically both 44100, so it runs at unity.

**Frameskip may be inert.** `RETRO_ENVIRONMENT_SET_AUDIO_BUFFER_STATUS_CALLBACK`
(62) is not implemented, so cores have no buffer information to skip against. It
was implemented once and reverted: pcsx_rearmed did register the callback, but
nothing changed, because skipping avoids *rendering* while the cost here is
emulation plus present.

**`getUsage()` is not the whole story.** CPU sitting at 70-78% while the frame
overruns means the loop is blocked, not compute-bound. That was the clue that
sent us to the present cost.

## Already ruled out for PS1 (pcsx_rearmed, my355)

Measured on device, not judged by ear. Recorded so none of it gets
re-investigated. The two real causes were frame pacing truncation (`d9f49ca`,
cinematics) and the `Crisp` present cost (`9a982b8`, gameplay).

| Tried | Result |
|---|---|
| Threaded SPU | 10.4 → 10.1ms emulation. No effect — reverted (`41fb011`) |
| Audio Reverb off | ~10.0ms. No effect |
| Sound Interpolation (gaussian) | no audible difference |
| Instruction Cache off | −1.7ms. Real, but an accuracy tradeoff and not enough alone |
| CPU Performance | ~+2fps. Small, and costs battery on every title |
| Frameskip (all 3 modes) | inert, even with callback 62 implemented |
| Prevent Tearing off | no change, and no tearing appeared either |
| Bad ROM dumps | mixed formats (bin/cue, pbp, chd) all affected equally |
| Resampler quality / sample-rate mismatch | in and out are both 44100; it runs at unity |
| Oversleeping in the frame limiter | `sleep=0.00ms` during gameplay — the loop never idled |

Two fixes found along the way that were unrelated to audio: the overlay crash in
`Menu_scale` (`baae017`, bezel viewport applied to the half-size savestate
thumbnail) and the pacing truncation itself, which affected every core.

## Checklist for the next "audio" bug

1. Build with `DEBUG=1` and capture a log. Do not theorise first.
2. `under=` or `drop=`? That splits starving from overflowing — opposite causes.
3. Check `fps`. Below ~59.4 sustained, it is a performance problem and no audio
   setting will fix it.
4. Split the frame: `run - flip` vs `flip`. Emulation and present fail
   differently and have different fixes.
5. Only then look at audio settings, and change one thing at a time.

Four hypotheses were wrong before this discipline was applied — and one of them
was committed as a default before being measured, then reverted (`41fb011`). Two
log lines found the real causes in two test runs.

## Related

- `TODO.md` — open items, including the `Crisp` cost and the broken
  `minarch_thread_video` (untracked, local notes)
