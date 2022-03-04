# parse xml file
import xml.etree.ElementTree as ET
import sys

def getTimeSecond(nsString):
    if nsString.endswith('ns'):
        return float(nsString[:-2]) / 1000000000
    if nsString.endswith('us'):
        return float(nsString[:-2]) / 1000000
    if nsString.endswith('ms'):
        return float(nsString[:-2]) / 1000
    if nsString.endswith('s'):
        return float(nsString[:-1])

def parseXML(xmlfile):
    tree = ET.parse(xmlfile)

    root = tree.getroot()

    flowOutputFile = open(xmlfile.replace('.xml', '.txt'), 'w')
    outputbuffer = ""

    simulationTime = 19

    flowlist = []

    for child in root:
        if child.tag != "FlowStats":
            continue
        for i, flow in enumerate(child):
            # if i>= 10:  # without ack flow
            #     break
            # if (getTimeSecond(flow.attrib['timeLastRxPacket']) - getTimeSecond(flow.attrib['timeFirstRxPacket'])) <= 0:
            #     continue
            flowdict = {}
            # print(f"Flow ID: {flow.attrib['flowId']}")
            outputbuffer += f"Flow ID: {flow.attrib['flowId']}\n"
            flowdict['flowId'] = flow.attrib['flowId']
            # print(f"Delay sum : {flow.attrib['delaySum']}")
            outputbuffer += f"Delay sum : {flow.attrib['delaySum']}\n"
            flowdict['delaySum'] = getTimeSecond(flow.attrib['delaySum'])
            # print(f"txPackets : {flow.attrib['txPackets']}")
            outputbuffer += f"txPackets : {flow.attrib['txPackets']}\n"
            flowdict['txPackets'] = int(flow.attrib['txPackets'])
            # print(f"rxPackets : {flow.attrib['rxPackets']}")
            outputbuffer += f"rxPackets : {flow.attrib['rxPackets']}\n"
            flowdict['rxPackets'] = int(flow.attrib['rxPackets'])
            flowdict['lostPackets'] = int(flow.attrib['txPackets'])-int(flow.attrib['rxPackets'])
            # print(f"lostPackets : {flowdict['lostPackets']}")
            outputbuffer += f"lostPackets : {flowdict['lostPackets']}\n"
            flowdict['deliveryRatio'] = round((int(flow.attrib['rxPackets'])/int(flow.attrib['txPackets']))*100, 5)
            # print(f"deliveryRatio : {flowdict['deliveryRatio']}")
            outputbuffer += f"deliveryRatio : {flowdict['deliveryRatio']}\n"
            flowdict['lossRatio'] = round((flowdict['lostPackets']/int(flow.attrib['txPackets']))*100, 5)
            # print(f"lossRatio : {flowdict['lossRatio']}")
            outputbuffer += f"lossRatio : {flowdict['lossRatio']}\n"
            flowdict['rxBytes'] = int(flow.attrib['rxBytes'])
            flowdict['duration'] = getTimeSecond(flow.attrib['timeLastRxPacket']) - getTimeSecond(flow.attrib['timeFirstRxPacket'])
            outputbuffer += f"duration : {flowdict['duration']}\n"
            try:
                flowdict['throughput'] = round(int(flow.attrib['rxBytes'])*8.0/((getTimeSecond(flow.attrib['timeLastRxPacket']) - getTimeSecond(flow.attrib['timeFirstRxPacket'])) * 1024), 5)
            except Exception as e:
                print(e)
                flowdict['throughput'] = 0
            # print(f"throughput : {flowdict['throughput']} kbps")
            outputbuffer += f"throughput : {flowdict['throughput']} kbps\n"
            # print()
            outputbuffer += "\n"
            flowlist.append(flowdict)
    
    totalFlowdict = {}
    totalFlowdict['flowIdCount'] = len(flowlist)
    totalFlowdict['delaySum'] = 0
    totalFlowdict['txPackets'] = 0
    totalFlowdict['rxPackets'] = 0
    totalFlowdict['lostPackets'] = 0
    totalFlowdict['deliveryRatio'] = 0
    totalFlowdict['lossRatio'] = 0
    totalFlowdict['throughput'] = 0
    totalFlowdict['rxBytes'] = 0
    totalFlowdict['duration'] = 0
    for flow in flowlist:
        totalFlowdict['delaySum'] += flow['delaySum']
        totalFlowdict['txPackets'] += flow['txPackets']
        totalFlowdict['rxPackets'] += flow['rxPackets']
        totalFlowdict['lostPackets'] += flow['lostPackets']
        totalFlowdict['throughput'] += flow['throughput']
        totalFlowdict['rxBytes'] += flow['rxBytes']
        totalFlowdict['duration'] += flow['duration']
    totalFlowdict['deliveryRatio'] = round((totalFlowdict['rxPackets']/totalFlowdict['txPackets'])*100, 5)
    totalFlowdict['lossRatio'] = round((totalFlowdict['lostPackets']/totalFlowdict['txPackets'])*100, 5)
    totalFlowdict['throughput'] = totalFlowdict['throughput'] / len(flowlist)

    outputbuffer += f"Total Flow Count: {totalFlowdict['flowIdCount']}\n"
    outputbuffer += f"Total Tx Packets: {totalFlowdict['txPackets']}\n"
    outputbuffer += f"Total Rx Packets: {totalFlowdict['rxPackets']}\n"
    outputbuffer += f"Total Lost Packets: {totalFlowdict['lostPackets']}\n"
    outputbuffer += f"Average Delivery Ratio: {totalFlowdict['deliveryRatio']}\n"
    outputbuffer += f"Average Loss Ratio: {totalFlowdict['lossRatio']}\n"
    outputbuffer += f"Average Throughput: {totalFlowdict['throughput']} kbps\n"
    outputbuffer += f"Average Throughput (proper): {round(totalFlowdict['rxBytes']*8.0 / (totalFlowdict['duration'] * 1024),5)} kbps\n"
    outputbuffer += f"Total Delay Sum: {totalFlowdict['delaySum']}\n"
    outputbuffer += f"Average end-to-end delay: {totalFlowdict['delaySum']/totalFlowdict['rxPackets']}\n"
    outputbuffer += f"net_throughput,{round(totalFlowdict['rxBytes']*8.0 / (simulationTime * 1024),5)} kbps\n"
    flowOutputFile.write(outputbuffer)
    flowOutputFile.close()

    csvfile = open(xmlfile.replace('.xml', '.csv'), 'w')
    csvbuffer = ""

    csvbuffer += f"total_flow_count,{totalFlowdict['flowIdCount']}\n"
    csvbuffer += f"total_tx_packets,{totalFlowdict['txPackets']}\n"
    csvbuffer += f"total_rx_packets,{totalFlowdict['rxPackets']}\n"
    csvbuffer += f"total_lost_packets,{totalFlowdict['lostPackets']}\n"
    csvbuffer += f"avg_delivery_ratio,{totalFlowdict['deliveryRatio']}\n"
    csvbuffer += f"avg_loss_ratio,{totalFlowdict['lossRatio']}\n"
    csvbuffer += f"avg_throughput,{totalFlowdict['throughput']}\n"
    csvbuffer += f"avg_throughput_proper,{round(totalFlowdict['rxBytes']*8.0 / (totalFlowdict['duration'] * 1024),5)}\n"
    csvbuffer += f"avg_end_to_end_delay,{totalFlowdict['delaySum']/totalFlowdict['rxPackets']}\n"
    csvbuffer += f"net_throughput,{round(totalFlowdict['rxBytes']*8.0 / (simulationTime * 1024),5)}\n"
    csvfile.write(csvbuffer)
    csvfile.close()

if __name__ == "__main__":
    xmlfile = sys.argv[1]
    parseXML(xmlfile)