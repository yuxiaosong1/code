# code
some code that usefully

# parseNetstat
usage:
	cat /proc/net/netstat | ./parseNetstat [TcpExt/IpExt/MPTcpExt]

# lttng_kernel.sh
usage:
	sudo ./lttng_kernel.sh {s | d} test_trace sched_switch,open
