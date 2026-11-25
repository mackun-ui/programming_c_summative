# Linux System Health and Resource Monitoring Manager

An interactive bash script that monitors CPU, memory, disk and processes on a Linux system, generates health reports and keeps an activity log.

## How to run
Run it in a Linux terminal (I used Ubuntu on WSL):
```
chmod +x system_monitor.sh
./system_monitor.sh
```

## Menu options
1. Display system information
2. Monitor CPU utilization
3. Monitor memory utilization
4. Monitor disk utilization
5. View and search running processes
6. Terminate a process
7. Generate a system health report
8. View activity history
9. Clear activity log
10. Exit

## Folders
- `reports/`: sample generated health report
- `docs/`: technical documentation and a sample activity log
- `screenshots/`: evidence of the script running