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

# ---------- 3. Memory monitoring ----------
monitor_memory() {
    echo "===== MEMORY UTILIZATION ====="
    if ! check_command free; then
        return 1
    fi

    local total used available percent
    total=$(free -m | awk '/^Mem:/ {print $2}')
    available=$(free -m | awk '/^Mem:/ {print $7}')

    if [ -z "$total" ] || [ "$total" -eq 0 ]; then
        echo "Cannot read memory information."
        log_activity "ERROR: could not read memory information"
        return 1
    fi

    used=$((total - available))
    percent=$((100 * used / total))

    echo "Total: ${total} MB"
    echo "Used: ${used} MB"
    echo "Available: ${available} MB"
    echo "Usage: ${percent}%"

    read -r -p "Memory warning threshold in % [current: ${MEM_THRESHOLD}] (Enter to keep): " input
    if [ -n "$input" ]; then
        if is_valid_threshold "$input"; then
            MEM_THRESHOLD=$input
        else
            echo "Invalid threshold. Keeping ${MEM_THRESHOLD}%."
        fi
    fi

    if [ "$percent" -ge "$MEM_THRESHOLD" ]; then
        echo "WARNING: memory usage (${percent}%) is above the threshold (${MEM_THRESHOLD}%)!"
        log_activity "WARNING: memory usage ${percent}% above threshold ${MEM_THRESHOLD}%"
    else
        echo "Memory usage is within the threshold (${MEM_THRESHOLD}%)."
    fi
    log_activity "Monitored memory utilization (${percent}%)"
}

# ---------- 4. Disk monitoring ----------
monitor_disk() {
    echo "===== DISK UTILISATION ====="
    if ! check_command df; then
        return 1
    fi

    echo "Mounted filesystems:"
    df -h -x tmpfs -x devtmpfs -x squashfs

    read -r -p "Disk warning threshold in % [current: ${DISK_THRESHOLD}] (Enter to keep): " input
    if [ -n "$input" ]; then
        if is_valid_threshold "$input"; then
            DISK_THRESHOLD=$input
        else
            echo "Invalid threshold. Keeping ${DISK_THRESHOLD}%."
        fi
    fi

    echo
    local found=0
    while read -r fs size used avail pct mount; do
        local pct_num=${pct%\%}
        if [[ "$pct_num" =~ ^[0-9]+$ ]] && [ "$pct_num" -ge "$DISK_THRESHOLD" ]; then
            echo "WARNING: $mount ($fs) is ${pct_num}% full"
            log_activity "WARNING: disk $mount at ${pct_num}% above threshold ${DISK_THRESHOLD}%"
            found=1
        fi
    done < <(df -hP -x tmpfs -x devtmpfs -x squashfs | tail -n +2)

    if [ "$found" -eq 0 ]; then
        echo "All filesystems are below ${DISK_THRESHOLD}%."
    fi
    log_activity "Monitored disk utilization"
}

# ---------- 5. Process management ----------
PROTECTED_NAMES="systemd init sshd kthreadd login dbus-daemon"

# Shows the top running processes
view_processes() {
    echo "===== RUNNING PROCESSES (top 15 by CPU) ====="
    ps -eo pid,user,%cpu,%mem,comm --sort=-%cpu | head -n 16
    log_activity "Viewed running processes"
}

# Searches by PID (numbers) or by name (text)
search_process() {
    read -r -p "Enter a process name or PID: " query
    if [ -z "$query" ]; then
        echo "Error: nothing entered."
        return 1
    fi

    if [[ "$query" =~ ^[0-9]+$ ]]; then
        if ps -p "$query" > /dev/null 2>&1; then
            ps -p "$query" -o pid,user,%cpu,%mem,comm
            log_activity "Searched for PID $query (found)"
        else
            echo "No process found with PID $query."
            log_activity "Searched for PID $query (not found)"
        fi
    else
        local matches
        matches=$(ps -eo pid,user,%cpu,%mem,comm | awk -v q="$query" 'NR>1 && index(tolower($5), tolower(q)) > 0')
        if [ -n "$matches" ]; then
            echo "PID USER %CPU %MEM COMMAND"
            echo "$matches"
            log_activity "Searched for process '$query' (found)"
        else
            echo "No process found matching '$query'."
            log_activity "Searched for process '$query' (not found)"
        fi
    fi
}

# Terminates a process after safety checks and confirmation
terminate_process() {
    read -r -p "Enter the PID to terminate: " pid
    if ! [[ "$pid" =~ ^[0-9]+$ ]]; then
        echo "Error: invalid PID."
        return 1
    fi
    if ! ps -p "$pid" > /dev/null 2>&1; then
        echo "Error: no process with PID $pid."
        log_activity "ERROR: terminate failed, PID $pid not found"
        return 1
    fi

    local name owner
    name=$(ps -p "$pid" -o comm= | tr -d ' ')
    owner=$(ps -p "$pid" -o user= | tr -d ' ')

    # Protected processes
    if [ "$pid" -le 1 ] || [ "$pid" -eq $$ ] || [ "$pid" -eq "$PPID" ]; then
        echo "Error: PID $pid is protected and cannot be terminated."
        log_activity "BLOCKED: attempt to terminate protected PID $pid"
        return 1
    fi
    for p in $PROTECTED_NAMES; do
        if [ "$name" = "$p" ]; then
            echo "Error: '$name' is a protected system process."
            log_activity "BLOCKED: attempt to terminate protected process $name"
            return 1
        fi
    done

    # Permission check
    if [ "$owner" != "$(whoami)" ]; then
        echo "Error: you do not have permission to terminate processes owned by '$owner'."
        log_activity "BLOCKED: no permission to terminate PID $pid ($owner)"
        return 1
    fi

    read -r -p "Terminate '$name' (PID $pid)? [y/n]: " answer
    if [[ "$answer" =~ ^[Yy]$ ]]; then
        if kill "$pid" 2>/dev/null; then
            echo "Process $pid terminated."
            log_activity "Terminated process $name (PID $pid)"
        else
            echo "Error: could not terminate process $pid."
            log_activity "ERROR: failed to terminate PID $pid"
        fi
    else
        echo "Cancelled."
        log_activity "Cancelled termination of PID $pid"
    fi
}

# ---------- 6. System health report ----------
generate_report() {
    echo "===== GENERATE SYSTEM HEALTH REPORT ====="
    read -r -p "Directory to save the report [${REPORT_DIR}]: " dir
    dir=${dir:-$REPORT_DIR}

    # Create the directory if it does not exist
    if [ ! -d "$dir" ]; then
        read -r -p "'$dir' does not exist. Create it? [y/n]: " answer
        if [[ "$answer" =~ ^[Yy]$ ]]; then
            if ! mkdir -p "$dir" 2>/dev/null; then
                echo "Error: could not create '$dir'."
                log_activity "ERROR: could not create report directory $dir"
                return 1
            fi
        else
            echo "Cancelled."
            return 1
        fi
    fi

    if [ ! -w "$dir" ]; then
        echo "Error: no permission to write in '$dir'."
        log_activity "ERROR: no write permission for $dir"
        return 1
    fi

    if ! check_command free || ! check_command df; then
        return 1
    fi

    local file cpu total available used percent
    file="$dir/health_report_$(date '+%Y%m%d_%H%M%S').txt"
    cpu=$(get_cpu_usage)
    total=$(free -m | awk '/^Mem:/ {print $2}')
    available=$(free -m | awk '/^Mem:/ {print $7}')
    used=$((total - available))
    percent=$((100 * used / total))

    {
        echo "===== SYSTEM HEALTH REPORT ====="
        echo "Generated: $(date '+%Y-%m-%d %H:%M:%S')"
        echo "Hostname: $(hostname)"
        echo "Uptime: $(uptime -p)"
        echo
        echo "--- CPU ---"
        echo "Usage: ${cpu}% (threshold ${CPU_THRESHOLD}%)"
        [ "$cpu" -ge "$CPU_THRESHOLD" ] && echo "WARNING: CPU usage above threshold"
        echo
        echo "--- Memory ---"
        echo "Total: ${total} MB | Used: ${used} MB | Available: ${available} MB"
        echo "Usage: ${percent}% (threshold ${MEM_THRESHOLD}%)"
        [ "$percent" -ge "$MEM_THRESHOLD" ] && echo "WARNING: memory usage above threshold"
        echo
        echo "--- Disk ---"
        df -h -x tmpfs -x devtmpfs -x squashfs
    } > "$file" 2>/dev/null

    if [ -s "$file" ]; then
        echo "Report saved to: $file"
        log_activity "Generated health report: $file"
    else
        echo "Error: could not write the report."
        log_activity "ERROR: failed to write report $file"
        return 1
    fi
}

# ---------- 7. Activity log ----------
view_log() {
    echo "===== ACTIVITY LOG ====="
    if [ ! -s "$LOG_FILE" ]; then
        echo "The activity log is empty."
        return 0
    fi
    cat "$LOG_FILE"
}

clear_log() {
    if [ ! -s "$LOG_FILE" ]; then
        echo "The activity log is already empty."
        return 0
    fi
    read -r -p "Clear the whole activity log? [y/n]: " answer
    if [[ "$answer" =~ ^[Yy]$ ]]; then
        if : > "$LOG_FILE" 2>/dev/null; then
            echo "Activity log cleared."
            log_activity "Activity log cleared"
        else
            echo "Error: could not clear the log."
        fi
    else
        echo "Cancelled."
    fi
}

# ---------- Startup check for required commands ----------
check_requirements() {
    local missing=0
    for cmd in ps df free awk uptime hostname uname; do
        if ! check_command "$cmd"; then
            missing=1
        fi
    done
    return $missing
}

# ---------- View processes, then optionally search ----------
view_and_search_processes() {
    view_processes
    echo
    read -r -p "Search for a specific process? [y/n]: " answer
    if [[ "$answer" =~ ^[Yy]$ ]]; then
        search_process
    fi
}

# ---------- 8. Interactive menu ----------
show_menu() {
    echo
    echo "===== SYSTEM HEALTH MONITOR ====="
    echo "1. Display system information"
    echo "2. Monitor CPU utilization"
    echo "3. Monitor memory utilization"
    echo "4. Monitor disk utilization"
    echo "5. View and search running processes"
    echo "6. Terminate a process"
    echo "7. Generate a system health report"
    echo "8. View activity history"
    echo "9. Clear activity log"
    echo "10. Exit"
}

main() {
    if ! check_requirements; then
        echo "Warning: some commands are missing, so some options may not work."
    fi
    log_activity "Application started"

    while true; do
        show_menu
        read -r -p "Choose an option [1-10]: " choice
        echo
        case "$choice" in
            1) show_system_info ;;
            2) monitor_cpu ;;
            3) monitor_memory ;;
            4) monitor_disk ;;
            5) view_and_search_processes ;;
            6) terminate_process ;;
            7) generate_report ;;
            8) view_log ;;
            9) clear_log ;;
            10) log_activity "Application closed"
                echo "Goodbye."
                exit 0 ;;
            *) echo "Invalid selection. Enter a number from 1 to 10." ;;
        esac
        echo
        pause
    done
}

main