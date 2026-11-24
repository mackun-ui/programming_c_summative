#!/bin/bash
# =====================================================
# Linux System Health and Resource Monitoring Manager
# =====================================================

# The rest of the script will be added step by step

# ---------- Settings ----------
LOG_FILE="activity.log"
CPU_THRESHOLD=80
MEM_THRESHOLD=80
DISK_THRESHOLD=80
REPORT_DIR="./reports"

# ---------- Activity log ----------
# Adds a timestamped line to the log file
log_activity() {
    local message="$1"
    echo "$(date '+%Y-%m-%d %H:%M:%S') | $message" >> "$LOG_FILE"
}

# ---------- Check that a command exists ----------
check_command() {
    if ! command -v "$1" > /dev/null 2>&1; then
        echo "Error: required command '$1' is not available."
        log_activity "ERROR: missing command $1"
        return 1
    fi
    return 0
}

# ---------- Pause helper ----------
pause() {
    read -r -p "Press Enter to continue..."
}

# ---------- 1. System information ----------
show_system_info() {
    echo "===== SYSTEM INFORMATION ====="
    echo "Hostname: $(hostname)"

    if [ -f /etc/os-release ]; then
        os_name=$(grep PRETTY_NAME /etc/os-release | cut -d= -f2 | tr -d '"')
    else
        os_name=$(uname -s)
    fi 
    echo "OS: $os_name"

    echo "Kernel: $(uname -r)"
    echo "Uptime: $(uptime -p 2>/dev/null || uptime)"
    echo "Date/Time: $(date '+%Y-%m-%d %H:%M:%S')"
    log_activity "Viewed system information"
}

# ---------- Threshold validation (reused for CPU, memory, disk) ----------
# A valid threshold is a whole number from 1 to 100
is_valid_threshold() {
    [[ "$1" =~ ^[0-9]+$ ]] && [ "$1" -ge 1 ] && [ "$1" -le 100 ]
}

# ---------- Reads CPU usage in % (two samples, 1 second apart) ----------
get_cpu_usage() {
    read -r _ u1 n1 s1 i1 w1 q1 sq1 st1 _ < /proc/stat
    sleep 1
    read -r _ u2 n2 s2 i2 w2 q2 sq2 st2 _ < /proc/stat

    local idle1=$((i1 + w1))
    local idle2=$((i2 + w2))
    local total1=$((u1 + n1 + s1 + i1 + w1 + q1 + sq1 + st1))
    local total2=$((u2 + n2 + s2 + i2 + w2 + q2 + sq2 + st2))
    local total_diff=$((total2 - total1))
    local idle_diff=$((idle2 - idle1))

    if [ "$total_diff" -eq 0 ]; then 
        echo 0
    else
        echo $(( 100 * (total_diff - idle_diff) / total_diff ))
    fi
}

# ---------- 2. CPU monitoring ----------
monitor_cpu() {
    echo "===== CPU UTILISATION ====="
    if [ ! -r /proc/stat ] || ! check_command ps; then
        echo "Cannot read CPU information on this system."
        return 1
    fi

    local usage
    usage=$(get_cpu_usage)
    echo "Current CPU usage: ${usage}%"

    read -r -p "CPU warning threshold in % [current: ${CPU_THRESHOLD}] (Enter to keep): " input
    if [ -n "$input" ]; then
        if is_valid_threshold "$input"; then
            CPU_THRESHOLD=$input
        else
            echo "Invalid threshold. Keeping ${CPU_THRESHOLD}%."
        fi
    fi

    if [ "$usage" -ge "$CPU_THRESHOLD" ]; then
        echo "WARNING: CPU usage (${usage}%) is above the threshold (${CPU_THRESHOLD}%)!"
        log_activity "WARNING: CPU usage ${usage}% above threshold ${CPU_THRESHOLD}%"
    else
        echo "CPU usage is within the threshold (${CPU_THRESHOLD}%)."
    fi 

    echo
    echo "Top 5 CPU-consuming processes:"
    ps -eo pid,comm,%cpu --sort=-%cpu | head -n 6
    log_activity "Monitored CPU utilization (${usage}%)"
}

# ---------- Temporary test (delete after testing) ----------
monitor_cpu
cat "$LOG_FILE"