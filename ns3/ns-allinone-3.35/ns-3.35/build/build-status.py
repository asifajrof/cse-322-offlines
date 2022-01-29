#! /usr/bin/env python3

# Programs that are runnable.
ns3_runnable_programs = ['build/src/aodv/examples/ns3.35-aodv-debug', 'build/src/applications/examples/ns3.35-three-gpp-http-example-debug', 'build/src/bridge/examples/ns3.35-csma-bridge-debug', 'build/src/bridge/examples/ns3.35-csma-bridge-one-hop-debug', 'build/src/buildings/examples/ns3.35-buildings-pathloss-profiler-debug', 'build/src/buildings/examples/ns3.35-outdoor-random-walk-example-debug', 'build/src/buildings/examples/ns3.35-outdoor-group-mobility-example-debug', 'build/src/config-store/examples/ns3.35-config-store-save-debug', 'build/src/core/examples/ns3.35-main-callback-debug', 'build/src/core/examples/ns3.35-sample-simulator-debug', 'build/src/core/examples/ns3.35-main-ptr-debug', 'build/src/core/examples/ns3.35-main-random-variable-stream-debug', 'build/src/core/examples/ns3.35-sample-random-variable-debug', 'build/src/core/examples/ns3.35-sample-random-variable-stream-debug', 'build/src/core/examples/ns3.35-command-line-example-debug', 'build/src/core/examples/ns3.35-hash-example-debug', 'build/src/core/examples/ns3.35-sample-log-time-format-debug', 'build/src/core/examples/ns3.35-test-string-value-formatting-debug', 'build/src/core/examples/ns3.35-sample-show-progress-debug', 'build/src/core/examples/ns3.35-empirical-random-variable-example-debug', 'build/src/core/examples/ns3.35-system-path-examples-debug', 'build/src/core/examples/ns3.35-fatal-example-debug', 'build/src/core/examples/ns3.35-main-test-sync-debug', 'build/src/core/examples/ns3.35-length-example-debug', 'build/src/csma/examples/ns3.35-csma-one-subnet-debug', 'build/src/csma/examples/ns3.35-csma-broadcast-debug', 'build/src/csma/examples/ns3.35-csma-packet-socket-debug', 'build/src/csma/examples/ns3.35-csma-multicast-debug', 'build/src/csma/examples/ns3.35-csma-raw-ip-socket-debug', 'build/src/csma/examples/ns3.35-csma-ping-debug', 'build/src/csma-layout/examples/ns3.35-csma-star-debug', 'build/src/dsdv/examples/ns3.35-dsdv-manet-debug', 'build/src/dsr/examples/ns3.35-dsr-debug', 'build/src/energy/examples/ns3.35-li-ion-energy-source-debug', 'build/src/energy/examples/ns3.35-rv-battery-model-test-debug', 'build/src/energy/examples/ns3.35-basic-energy-model-test-debug', 'build/src/fd-net-device/examples/ns3.35-dummy-network-debug', 'build/src/fd-net-device/examples/ns3.35-fd2fd-onoff-debug', 'build/src/fd-net-device/examples/ns3.35-realtime-dummy-network-debug', 'build/src/fd-net-device/examples/ns3.35-realtime-fd2fd-onoff-debug', 'build/src/fd-net-device/examples/ns3.35-fd-emu-ping-debug', 'build/src/fd-net-device/examples/ns3.35-fd-emu-onoff-debug', 'build/src/fd-net-device/examples/ns3.35-fd-emu-tc-debug', 'build/src/fd-net-device/examples/ns3.35-fd-emu-send-debug', 'build/src/fd-net-device/examples/ns3.35-fd-emu-udp-echo-debug', 'build/src/fd-net-device/examples/ns3.35-fd-tap-ping-debug', 'build/src/fd-net-device/examples/ns3.35-fd-tap-ping6-debug', 'build/src/internet/examples/ns3.35-main-simple-debug', 'build/src/internet-apps/examples/ns3.35-dhcp-example-debug', 'build/src/internet-apps/examples/ns3.35-traceroute-example-debug', 'build/src/lr-wpan/examples/ns3.35-lr-wpan-packet-print-debug', 'build/src/lr-wpan/examples/ns3.35-lr-wpan-phy-test-debug', 'build/src/lr-wpan/examples/ns3.35-lr-wpan-data-debug', 'build/src/lr-wpan/examples/ns3.35-lr-wpan-error-model-plot-debug', 'build/src/lr-wpan/examples/ns3.35-lr-wpan-error-distance-plot-debug', 'build/src/lr-wpan/examples/ns3.35-lr-wpan-mlme-debug', 'build/src/lte/examples/ns3.35-lena-cqi-threshold-debug', 'build/src/lte/examples/ns3.35-lena-dual-stripe-debug', 'build/src/lte/examples/ns3.35-lena-fading-debug', 'build/src/lte/examples/ns3.35-lena-intercell-interference-debug', 'build/src/lte/examples/ns3.35-lena-ipv6-addr-conf-debug', 'build/src/lte/examples/ns3.35-lena-ipv6-ue-rh-debug', 'build/src/lte/examples/ns3.35-lena-ipv6-ue-ue-debug', 'build/src/lte/examples/ns3.35-lena-pathloss-traces-debug', 'build/src/lte/examples/ns3.35-lena-profiling-debug', 'build/src/lte/examples/ns3.35-lena-rem-debug', 'build/src/lte/examples/ns3.35-lena-rem-sector-antenna-debug', 'build/src/lte/examples/ns3.35-lena-rlc-traces-debug', 'build/src/lte/examples/ns3.35-lena-simple-debug', 'build/src/lte/examples/ns3.35-lena-simple-epc-debug', 'build/src/lte/examples/ns3.35-lena-simple-epc-backhaul-debug', 'build/src/lte/examples/ns3.35-lena-deactivate-bearer-debug', 'build/src/lte/examples/ns3.35-lena-x2-handover-debug', 'build/src/lte/examples/ns3.35-lena-x2-handover-measures-debug', 'build/src/lte/examples/ns3.35-lena-frequency-reuse-debug', 'build/src/lte/examples/ns3.35-lena-distributed-ffr-debug', 'build/src/lte/examples/ns3.35-lena-uplink-power-control-debug', 'build/src/lte/examples/ns3.35-lena-radio-link-failure-debug', 'build/src/lte/examples/ns3.35-lena-simple-epc-emu-debug', 'build/src/mesh/examples/ns3.35-mesh-debug', 'build/src/mobility/examples/ns3.35-main-grid-topology-debug', 'build/src/mobility/examples/ns3.35-main-random-topology-debug', 'build/src/mobility/examples/ns3.35-main-random-walk-debug', 'build/src/mobility/examples/ns3.35-mobility-trace-example-debug', 'build/src/mobility/examples/ns3.35-ns2-mobility-trace-debug', 'build/src/mobility/examples/ns3.35-bonnmotion-ns2-example-debug', 'build/src/mobility/examples/ns3.35-reference-point-group-mobility-example-debug', 'build/src/netanim/examples/ns3.35-dumbbell-animation-debug', 'build/src/netanim/examples/ns3.35-grid-animation-debug', 'build/src/netanim/examples/ns3.35-star-animation-debug', 'build/src/netanim/examples/ns3.35-wireless-animation-debug', 'build/src/netanim/examples/ns3.35-uan-animation-debug', 'build/src/netanim/examples/ns3.35-colors-link-description-debug', 'build/src/netanim/examples/ns3.35-resources-counters-debug', 'build/src/network/examples/ns3.35-main-packet-header-debug', 'build/src/network/examples/ns3.35-main-packet-tag-debug', 'build/src/network/examples/ns3.35-packet-socket-apps-debug', 'build/src/network/examples/ns3.35-lollipop-comparisions-debug', 'build/src/network/examples/ns3.35-bit-serializer-debug', 'build/src/nix-vector-routing/examples/ns3.35-nix-simple-debug', 'build/src/nix-vector-routing/examples/ns3.35-nms-p2p-nix-debug', 'build/src/nix-vector-routing/examples/ns3.35-nix-simple-multi-address-debug', 'build/src/nix-vector-routing/examples/ns3.35-nix-double-wifi-debug', 'build/src/olsr/examples/ns3.35-simple-point-to-point-olsr-debug', 'build/src/olsr/examples/ns3.35-olsr-hna-debug', 'build/src/point-to-point/examples/ns3.35-main-attribute-value-debug', 'build/src/propagation/examples/ns3.35-main-propagation-loss-debug', 'build/src/propagation/examples/ns3.35-jakes-propagation-model-example-debug', 'build/src/sixlowpan/examples/ns3.35-example-sixlowpan-debug', 'build/src/sixlowpan/examples/ns3.35-example-ping-lr-wpan-debug', 'build/src/sixlowpan/examples/ns3.35-example-ping-lr-wpan-beacon-debug', 'build/src/sixlowpan/examples/ns3.35-example-ping-lr-wpan-mesh-under-debug', 'build/src/spectrum/examples/ns3.35-adhoc-aloha-ideal-phy-debug', 'build/src/spectrum/examples/ns3.35-adhoc-aloha-ideal-phy-matrix-propagation-loss-model-debug', 'build/src/spectrum/examples/ns3.35-adhoc-aloha-ideal-phy-with-microwave-oven-debug', 'build/src/spectrum/examples/ns3.35-tv-trans-example-debug', 'build/src/spectrum/examples/ns3.35-tv-trans-regional-example-debug', 'build/src/spectrum/examples/ns3.35-three-gpp-channel-example-debug', 'build/src/stats/examples/ns3.35-gnuplot-example-debug', 'build/src/stats/examples/ns3.35-double-probe-example-debug', 'build/src/stats/examples/ns3.35-time-probe-example-debug', 'build/src/stats/examples/ns3.35-gnuplot-aggregator-example-debug', 'build/src/stats/examples/ns3.35-gnuplot-helper-example-debug', 'build/src/stats/examples/ns3.35-file-aggregator-example-debug', 'build/src/stats/examples/ns3.35-file-helper-example-debug', 'build/src/tap-bridge/examples/ns3.35-tap-csma-debug', 'build/src/tap-bridge/examples/ns3.35-tap-csma-virtual-machine-debug', 'build/src/tap-bridge/examples/ns3.35-tap-wifi-virtual-machine-debug', 'build/src/tap-bridge/examples/ns3.35-tap-wifi-dumbbell-debug', 'build/src/topology-read/examples/ns3.35-topology-example-sim-debug', 'build/src/traffic-control/examples/ns3.35-red-tests-debug', 'build/src/traffic-control/examples/ns3.35-red-vs-ared-debug', 'build/src/traffic-control/examples/ns3.35-adaptive-red-tests-debug', 'build/src/traffic-control/examples/ns3.35-pfifo-vs-red-debug', 'build/src/traffic-control/examples/ns3.35-codel-vs-pfifo-basic-test-debug', 'build/src/traffic-control/examples/ns3.35-codel-vs-pfifo-asymmetric-debug', 'build/src/traffic-control/examples/ns3.35-pie-example-debug', 'build/src/traffic-control/examples/ns3.35-fqcodel-l4s-example-debug', 'build/src/uan/examples/ns3.35-uan-cw-example-debug', 'build/src/uan/examples/ns3.35-uan-rc-example-debug', 'build/src/uan/examples/ns3.35-uan-raw-example-debug', 'build/src/uan/examples/ns3.35-uan-ipv4-example-debug', 'build/src/uan/examples/ns3.35-uan-ipv6-example-debug', 'build/src/uan/examples/ns3.35-uan-6lowpan-example-debug', 'build/src/virtual-net-device/examples/ns3.35-virtual-net-device-debug', 'build/src/wave/examples/ns3.35-wave-simple-80211p-debug', 'build/src/wave/examples/ns3.35-wave-simple-device-debug', 'build/src/wave/examples/ns3.35-vanet-routing-compare-debug', 'build/src/wifi/examples/ns3.35-wifi-phy-test-debug', 'build/src/wifi/examples/ns3.35-wifi-test-interference-helper-debug', 'build/src/wifi/examples/ns3.35-wifi-manager-example-debug', 'build/src/wifi/examples/ns3.35-wifi-trans-example-debug', 'build/src/wifi/examples/ns3.35-wifi-phy-configuration-debug', 'build/src/wifi/examples/ns3.35-wifi-bianchi-debug', 'build/src/wimax/examples/ns3.35-wimax-ipv4-debug', 'build/src/wimax/examples/ns3.35-wimax-multicast-debug', 'build/src/wimax/examples/ns3.35-wimax-simple-debug', 'build/examples/channel-models/ns3.35-three-gpp-v2v-channel-example-debug', 'build/examples/energy/ns3.35-energy-model-example-debug', 'build/examples/energy/ns3.35-energy-model-with-harvesting-example-debug', 'build/examples/error-model/ns3.35-simple-error-model-debug', 'build/examples/ipv6/ns3.35-icmpv6-redirect-debug', 'build/examples/ipv6/ns3.35-ping6-debug', 'build/examples/ipv6/ns3.35-radvd-debug', 'build/examples/ipv6/ns3.35-radvd-two-prefix-debug', 'build/examples/ipv6/ns3.35-test-ipv6-debug', 'build/examples/ipv6/ns3.35-fragmentation-ipv6-debug', 'build/examples/ipv6/ns3.35-fragmentation-ipv6-two-MTU-debug', 'build/examples/ipv6/ns3.35-loose-routing-ipv6-debug', 'build/examples/ipv6/ns3.35-wsn-ping6-debug', 'build/examples/matrix-topology/ns3.35-matrix-topology-debug', 'build/examples/naming/ns3.35-object-names-debug', 'build/examples/realtime/ns3.35-realtime-udp-echo-debug', 'build/examples/routing/ns3.35-dynamic-global-routing-debug', 'build/examples/routing/ns3.35-static-routing-slash32-debug', 'build/examples/routing/ns3.35-global-routing-slash32-debug', 'build/examples/routing/ns3.35-global-injection-slash32-debug', 'build/examples/routing/ns3.35-simple-global-routing-debug', 'build/examples/routing/ns3.35-simple-alternate-routing-debug', 'build/examples/routing/ns3.35-mixed-global-routing-debug', 'build/examples/routing/ns3.35-simple-routing-ping6-debug', 'build/examples/routing/ns3.35-manet-routing-compare-debug', 'build/examples/routing/ns3.35-ripng-simple-network-debug', 'build/examples/routing/ns3.35-rip-simple-network-debug', 'build/examples/routing/ns3.35-global-routing-multi-switch-plus-router-debug', 'build/examples/routing/ns3.35-simple-multicast-flooding-debug', 'build/examples/socket/ns3.35-socket-bound-static-routing-debug', 'build/examples/socket/ns3.35-socket-bound-tcp-static-routing-debug', 'build/examples/socket/ns3.35-socket-options-ipv4-debug', 'build/examples/socket/ns3.35-socket-options-ipv6-debug', 'build/examples/stats/ns3.35-wifi-example-sim-debug', 'build/examples/tcp/ns3.35-tcp-large-transfer-debug', 'build/examples/tcp/ns3.35-tcp-nsc-lfn-debug', 'build/examples/tcp/ns3.35-tcp-nsc-zoo-debug', 'build/examples/tcp/ns3.35-tcp-star-server-debug', 'build/examples/tcp/ns3.35-star-debug', 'build/examples/tcp/ns3.35-tcp-bulk-send-debug', 'build/examples/tcp/ns3.35-tcp-pcap-nanosec-example-debug', 'build/examples/tcp/ns3.35-tcp-nsc-comparison-debug', 'build/examples/tcp/ns3.35-tcp-variants-comparison-debug', 'build/examples/tcp/ns3.35-tcp-pacing-debug', 'build/examples/tcp/ns3.35-dctcp-example-debug', 'build/examples/tcp/ns3.35-tcp-linux-reno-debug', 'build/examples/tcp/ns3.35-tcp-validation-debug', 'build/examples/tcp/ns3.35-tcp-bbr-example-debug', 'build/examples/traffic-control/ns3.35-traffic-control-debug', 'build/examples/traffic-control/ns3.35-queue-discs-benchmark-debug', 'build/examples/traffic-control/ns3.35-red-vs-fengadaptive-debug', 'build/examples/traffic-control/ns3.35-red-vs-nlred-debug', 'build/examples/traffic-control/ns3.35-tbf-example-debug', 'build/examples/traffic-control/ns3.35-cobalt-vs-codel-debug', 'build/examples/tutorial/ns3.35-hello-simulator-debug', 'build/examples/tutorial/ns3.35-first-debug', 'build/examples/tutorial/ns3.35-second-debug', 'build/examples/tutorial/ns3.35-third-debug', 'build/examples/tutorial/ns3.35-fourth-debug', 'build/examples/tutorial/ns3.35-fifth-debug', 'build/examples/tutorial/ns3.35-sixth-debug', 'build/examples/tutorial/ns3.35-seventh-debug', 'build/examples/udp/ns3.35-udp-echo-debug', 'build/examples/udp-client-server/ns3.35-udp-client-server-debug', 'build/examples/udp-client-server/ns3.35-udp-trace-client-server-debug', 'build/examples/wireless/ns3.35-mixed-wired-wireless-debug', 'build/examples/wireless/ns3.35-wifi-adhoc-debug', 'build/examples/wireless/ns3.35-wifi-clear-channel-cmu-debug', 'build/examples/wireless/ns3.35-wifi-ap-debug', 'build/examples/wireless/ns3.35-wifi-wired-bridging-debug', 'build/examples/wireless/ns3.35-wifi-multirate-debug', 'build/examples/wireless/ns3.35-wifi-simple-adhoc-debug', 'build/examples/wireless/ns3.35-wifi-simple-adhoc-grid-debug', 'build/examples/wireless/ns3.35-wifi-simple-infra-debug', 'build/examples/wireless/ns3.35-wifi-simple-interference-debug', 'build/examples/wireless/ns3.35-wifi-blockack-debug', 'build/examples/wireless/ns3.35-wifi-dsss-validation-debug', 'build/examples/wireless/ns3.35-wifi-ofdm-validation-debug', 'build/examples/wireless/ns3.35-wifi-ofdm-ht-validation-debug', 'build/examples/wireless/ns3.35-wifi-ofdm-vht-validation-debug', 'build/examples/wireless/ns3.35-wifi-hidden-terminal-debug', 'build/examples/wireless/ns3.35-wifi-ht-network-debug', 'build/examples/wireless/ns3.35-wifi-vht-network-debug', 'build/examples/wireless/ns3.35-wifi-timing-attributes-debug', 'build/examples/wireless/ns3.35-wifi-sleep-debug', 'build/examples/wireless/ns3.35-wifi-power-adaptation-distance-debug', 'build/examples/wireless/ns3.35-wifi-power-adaptation-interference-debug', 'build/examples/wireless/ns3.35-wifi-rate-adaptation-distance-debug', 'build/examples/wireless/ns3.35-wifi-aggregation-debug', 'build/examples/wireless/ns3.35-wifi-txop-aggregation-debug', 'build/examples/wireless/ns3.35-wifi-simple-ht-hidden-stations-debug', 'build/examples/wireless/ns3.35-wifi-80211n-mimo-debug', 'build/examples/wireless/ns3.35-wifi-mixed-network-debug', 'build/examples/wireless/ns3.35-wifi-tcp-debug', 'build/examples/wireless/ns3.35-wifi-80211e-txop-debug', 'build/examples/wireless/ns3.35-wifi-spectrum-per-example-debug', 'build/examples/wireless/ns3.35-wifi-spectrum-per-interference-debug', 'build/examples/wireless/ns3.35-wifi-spectrum-saturation-example-debug', 'build/examples/wireless/ns3.35-wifi-ofdm-he-validation-debug', 'build/examples/wireless/ns3.35-wifi-he-network-debug', 'build/examples/wireless/ns3.35-wifi-multi-tos-debug', 'build/examples/wireless/ns3.35-wifi-backward-compatibility-debug', 'build/examples/wireless/ns3.35-wifi-spatial-reuse-debug', 'build/examples/wireless/ns3.35-wifi-error-models-comparison-debug', 'build/scratch/ns3.35-first-debug', 'build/scratch/ns3.35-scratch-simulator-debug', 'build/scratch/ns3.35-seventh-debug', 'build/scratch/subdir/ns3.35-subdir-debug', 'build/scratch/ns3.35-test-debug']

# Scripts that are runnable.
ns3_runnable_scripts = ['csma-bridge.py', 'sample-simulator.py', 'wifi-olsr-flowmon.py', 'tap-csma-virtual-machine.py', 'tap-wifi-virtual-machine.py', 'realtime-udp-echo.py', 'simple-routing-ping6.py', 'first.py', 'second.py', 'third.py', 'mixed-wired-wireless.py', 'wifi-ap.py']

