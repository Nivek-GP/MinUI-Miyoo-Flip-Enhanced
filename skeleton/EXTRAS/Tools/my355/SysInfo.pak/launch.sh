#!/bin/sh

OUT="$SDCARD_PATH/sysinfo.txt"

{
echo "===== sysinfo.txt ====="
echo "date: $(date)"
echo ""

echo "--- uname ---"
uname -a
echo ""

echo "--- /etc/os-release ---"
cat /etc/os-release 2>/dev/null || echo "(not found)"
echo ""

echo "--- network interfaces ---"
ls /sys/class/net/
echo ""

echo "--- ifconfig ---"
ifconfig 2>/dev/null || ip addr 2>/dev/null || echo "(neither ifconfig nor ip found)"
echo ""

echo "--- wlan0 operstate ---"
cat /sys/class/net/wlan0/operstate 2>/dev/null || echo "(no wlan0)"
echo ""

echo "--- /proc/net/wireless ---"
cat /proc/net/wireless 2>/dev/null || echo "(not found)"
echo ""

echo "--- lsmod ---"
lsmod 2>/dev/null || cat /proc/modules 2>/dev/null || echo "(not found)"
echo ""

echo "--- kernel modules (.ko) ---"
find /lib/modules/ -name "*.ko" 2>/dev/null | sort || echo "(no /lib/modules)"
echo ""

echo "--- wifi-related binaries in PATH ---"
for dir in /usr/miyoo/bin /usr/miyoo/sbin /usr/bin /usr/sbin /bin /sbin; do
    if [ -d "$dir" ]; then
        found=$(ls "$dir" 2>/dev/null | grep -iE "wifi|wpa|hostapd|dhcp|udhcpc|adb|telnet|ssh")
        if [ -n "$found" ]; then
            echo "$dir: $found"
        fi
    fi
done
echo ""

echo "--- wpa_supplicant config ---"
for f in /appconfigs/wpa_supplicant.conf /etc/wpa_supplicant.conf /data/misc/wifi/wpa_supplicant.conf; do
    if [ -f "$f" ]; then
        echo "found: $f"
        cat "$f"
        echo ""
    fi
done
echo ""

echo "--- running processes (network-related) ---"
ps 2>/dev/null | grep -iE "wifi|wpa|dhcp|udhcpc|hostapd|adb|telnet|ssh" | grep -v grep || echo "(none found)"
echo ""

echo "--- all running processes ---"
ps 2>/dev/null || echo "(ps not available)"
echo ""

echo "--- /usr/miyoo/bin contents ---"
ls -la /usr/miyoo/bin/ 2>/dev/null || echo "(not found)"
echo ""

echo "--- /usr/miyoo/sbin contents ---"
ls -la /usr/miyoo/sbin/ 2>/dev/null || echo "(not found)"
echo ""

echo "--- /customer/app contents ---"
ls -la /customer/app/ 2>/dev/null || echo "(not found)"
echo ""

echo "===== end ====="
} > "$OUT" 2>&1

# brief pause so the user sees the tool ran (MinUI returns immediately otherwise)
sleep 1
