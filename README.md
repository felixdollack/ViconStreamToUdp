# ViconStreamToUdp
A command line program to receive data from a Vicon stream and forward it formatted as a UDP message.

The program is currently highly specific to my application,
but I hope it is helpful as starting point for someone.

It works together with the following to openFrameworks applications:
* https://github.com/felixdollack/phd_experiment_follow
* https://github.com/felixdollack/phd_experiment_find

## Setup
* Create a patient named "Felix" in Vicon.
* Create a segment called "Head" in patient Felix.
* Adapt the IP address in ViconDataStreamSDK_CPPTest/ForwardVicon.cpp in line 179 and recompile.
