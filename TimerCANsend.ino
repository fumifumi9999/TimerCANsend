#include "Arduino.h"
#include <DueTimer.h>

#include <due_can.h>
#include "variant.h"

int val = 0;

void fastHandler() { //100us Interrupt
	//Serial.println("[-  ] First Handler!");
}

void normalHandler() { //10ms Interrupt
	Serial.println("[-  ] slow Handler!");
	Serial.println("[ - ] send CAN frame!");
	sendStandardFrame();
	val = analogRead(0);
}

void slowHandler() { //1s Interrupt

}

void sendStandardFrame() {
	CAN_FRAME frame;
	frame.id = 0x400;
	frame.extended = false;
	frame.priority = 1; //0-15 lower is higher priority

	frame.data.s0 = val;
	frame.data.byte[2] = 0xDD;
	frame.data.byte[3] = 0x55;
	frame.data.high = 0xDEADBEEF;
	frame.length = 8;
	Can0.sendFrame(frame);
}

void printFrame(CAN_FRAME *frame, int filter) {
	Serial.print("Fltr: ");
	if (filter > -1) Serial.print(filter);
	else Serial.print("???");
	Serial.print(" ID: 0x");
	Serial.print(frame->id, HEX);
	Serial.print(" Len: ");
	Serial.print(frame->length);
	Serial.print(" Data: 0x");
	for (int count = 0; count < frame->length; count++) {
		Serial.print(frame->data.bytes[count], HEX);
		Serial.print(" ");
	}
	Serial.print("\r\n");
}

// Mail Boxs can be used up to 7 [0-6]
void gotFrameMB0(CAN_FRAME *frame) {
	Serial.println("[-  ] CAN recieved 0");

	printFrame(frame, 0);
}
void gotFrameMB3(CAN_FRAME *frame) {
	Serial.println("[-  ] CAN recieved 3");

	printFrame(frame, 3);
}
void gotFrame(CAN_FRAME *frame) {
	Serial.println("[ - ] Other CAN recieved");
	printFrame(frame, -1);
}

void setup() {
	Serial.begin(115200);
	analogReadResolution(12);

	Timer3.attachInterrupt(fastHandler);
	Timer4.attachInterrupt(normalHandler);
	Timer5.attachInterrupt(slowHandler);

	Can0.begin(CAN_BPS_500K);
	// when using Filter
	//Can0.setRXFilter(0, 0, false);             //catch all mailbox - no mailbox ID specified
//	Can0.setRXFilter(3, 0x400, 0x7FF, false);  //standard frame can be used up to ID 0x7FF
//	//Can0.watchFor(0x400, 0x401);
//	Can0.setCallback(0, gotFrameMB0);
//	Can0.setCallback(3, gotFrameMB3);
//	Can0.setGeneralCallback(gotFrame);

	//  // when using watchForRange
	//  Can0.watchForRange(0x300, 0x301);
	//  Can0.watchForRange(0x400, 0x401);
	//  Can0.setRXFilter(0, 0, false); //catch all mailbox
	//
	//  //now register all of the callback functions.
	//  Can0.attachCANInterrupt(0, gotFrameMB0);
	//  Can0.attachCANInterrupt(3, gotFrameMB3);
	//  Can0.attachCANInterrupt(gotFrame);

	Timer3.start(100);    //100us
	Timer4.start(100000);  //Fortest100ms
	Timer5.start(1000000);//1s
}

void loop() {
}
