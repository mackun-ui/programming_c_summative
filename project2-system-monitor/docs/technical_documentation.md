# Technical Documentation

## Structure
The script is split into one function per job, and `main` shows the menu in a loop and calls them.

- `log_activity`: adds a timestamped line to `activity.log`
- `check_command` and `check_requirements`: check that tools like `ps`, `df` and `free` exist
- `show_system_info`: hostname, OS, kernel, uptime, date and time
- `get_cpu_usage` and `monitor_cpu`: CPU usage, top 5 processes, user threshold
- `monitor_memory`: total, used and available memory with a percentage
- `monitor_disk`: used and free space per filesystem, warns above the threshold
- `view_processes`, `search_process`, `terminate_process`: process management
- `generate_report`: saves a timestamped report in a directory the user picks
- `view_log` and `clear_log`: activity log options
- `is_valid_threshold`: checks that a threshold is a number from 1 to 100

## Commands used
`hostname`, `uname`, `uptime`, `date`, `ps`, `free`, `df`, `awk`, `grep`, `kill`, `whoami`, and `/proc/stat` for CPU usage.

CPU usage is calculated from two readings of `/proc/stat` taken one second apart.

## Error handling
- Menu choices and thresholds are validated, and bad input gives a message instead of crashing.
- If a command is missing, the script says so and logs it.
- Invalid or missing PIDs are rejected.
- `terminate_process` refuses PID 1, the script itself, its parent, a list of protected system processes, and processes owned by another user. It also asks for confirmation first.
- Report generation checks that the directory exists, offers to create it, and checks write permission.
- Clearing the log needs confirmation.