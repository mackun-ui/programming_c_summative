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