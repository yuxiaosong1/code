#!/bin/bash

# lttng step:
# lttng create {session-name} --output=xxx
# lttng enable-event --kernel xxx
# lttng enable-event --kernel --syscall xxx
# lttng start
# lttng destroy

# scripts usage:
# sudo ./lttng_kernel.sh { s | d } {directory} {event_or_syscall}

if [ "$EUID" -ne 0 ]; then
	echo "Please run as root..."
	exit 0;
fi

# $1: s -- start  d -- destroy
# $2: output directory

directory=$2

output_path="/home/yuxiaosong/tmp_files/""$directory"

if [ "$1" == "d" ]; then
	lttng destroy && chown -R ${SUDO_USER} $output_path
	exit 0;
fi

if [ "$1" != "s" ]; then
	echo "invalid action, expect s or d"
	exit 0;
fi

# $3: kernel events or syscall that want to enable; only for start process
event_syscall=$3

if [ -d "$output_path" ]; then
	echo "remove existd output directory:""$output_path"
	rm -rf "$output_path"
fi

if lttng status 2> /dev/null | grep -q "kernel-session" ; then
	echo "kernel-session alreay existd"
	exit 0;
fi

lttng create kernel-session --output="$output_path"

event_tmp_file="/tmp/lttng_kernel_event_file"
syscall_tmp_file="/tmp/lttng_kernel_syscall_file"
lttng list --kernel > $event_tmp_file
lttng list --kernel --syscall > $syscall_tmp_file

event_str=""
syscall_str=""

IFS=',' read -ra arr <<< "$event_syscall"
for item in "${arr[@]}"; do
	if grep -rqn "$item" "$event_tmp_file" ; then
		event_str=$event_str"${item},"
		continue
	fi

	if grep -rqn "$item" "$syscall_tmp_file" ; then
		syscall_str=$syscall_str"${item},"
		continue
	fi

	echo "ignore invalid event or syscall: ""$item"
done

rm -rf $event_tmp_file
rm -rf $syscall_tmp_file

lttng enable-event --kernel $event_str
lttng enable-event --kernel --syscall $syscall_str

lttng start
