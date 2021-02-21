TIMEOUT(900000, log.log("Total PRR " + totalPRR + "\n"));

 packetsReceived= new Array();

packetsSent = new Array();

serverID = 1;

nodeCount = 18;

totalPRR = 0;
t_total = 0;

throughput = 0;

PDR=0;

data_length = 100;

Average_delay = 0;

timeReceived= new Array();

timeSent = new Array();

delay = new Array();

for(i = 0; i < ( nodeCount-1); i++) {

packetsReceived[i] = 0;

packetsSent[i] = 0;

timeReceived[i] = 0.0;

timeSent[i] = 0.0;

delay[i] = 0.0;

}

while(1) {

YIELD();

msgArray = msg.split(' ');

// log.log("string: "+msgArray+"\n");



if(msgArray[0].equals("receiver")) {

// Received packet

senderID = parseInt(msgArray[1]);

packetsReceived[senderID]++;

timeReceived[senderID] = time;

log.log("\n" + " SenderID " + senderID + " PRR " + packetsReceived[senderID] / packetsSent[senderID] + "timeReceived[senderID]" +  timeReceived[senderID] + " timeSent[senderID] " + timeSent[senderID] + "\n");

totalReceived = totalSent = 0;

totaldelay = 0;

count1 = 0;

for(i = 0; i < ( nodeCount-1); i++) 
{

    totalReceived += packetsReceived[i];

    totalSent += packetsSent[i];

    if (timeReceived[i] > 0) {

        delay[i] = timeReceived[i] - timeSent[i];

        delay[i] = delay[i] / 10000000 ;


        if (delay[i] > 0) 
        {

         

            totaldelay = totaldelay + delay[i];

            count1++;

        }

}

}

totalPRR = totalReceived / totalSent;

total_simulation_time=time;
log.log("\n"+"Total simulation time"+total_simulation_time+"\n");

throughput = (totalReceived * data_length * 8 *1000) / total_simulation_time;

log.log("\n"+"Total Received " + totalReceived + "totalSent" + totalSent + "\n");
PDR=(totalReceived / totalSent) * 100;

t_total = totalPRR * 100 ;

 

log.log("\n" + "Total Packet reception rate " + totalPRR + " Total_delay " + totaldelay + "Packet Delivery Ratio" + PDR + "\n");

log.log("\n"+ "Throughput "+throughput+"\n");

} 

else if(msgArray[0].equals("sender")) {

// Sent packet
receiverID = parseInt(msgArray[1]);


packetsSent[receiverID]++;

timeSent[receiverID] = time;

log.log( "\n" + " packetsSent[id]"  + packetsSent[receiverID]  + " timeSent[id] " + timeSent[receiverID] +  " id " + receiverID + "\n");

}



}