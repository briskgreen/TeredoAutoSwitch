# TeredoAutoSwitch
自动切换teredo服务器linux版

根据teredo服务器列表检查列表中服务器是否可用，向可用服务器地址发送icmp包检查响应时间，根据响应时间设置teredo服务器。

	*需要root权限

	*systemd

	*-m 选项修改配置文件，重启miredo服务（默认不修改）
