#include <NotifierClient.h>


static char send[] = "RANDOM TO SEND LOL IT BETTER TO BE AT LEAST TWO PACKETS RIGHT?!";
static char out[500] = {};

bool NotifierClient::sendFingerConfirmation(uint8_t fingerId, uint8_t confidence){
    return true;
}

bool NotifierClient::init(void) {
    return true;
}

bool NotifierClient::writeStructuredPacket(const Client_Packet &p) {
    return true;
}

bool NotifierClient::readStructuredPacket(const Client_Packet &p) {
    return true;
}

bool NotifierClient::sendMessage(void) {
    return client.accept(TEST_STRUCT, send, 64, false /* forced transmit */);
}

//TODO: implement this somehow
bool NotifierClient::readMessage() {
    /* we need some kind of locking mechanism because of interrupts */
    /* is it not possible to overwrite a message while it's being read, at least for slave */
    int16_t dataChecksum = client.readMessage(out /* read into */);
    
    return dataChecksum != -1;
}