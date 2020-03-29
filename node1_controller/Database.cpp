/*
    Database.cpp - For connecting to a Firebase Realtime database
    Created by Nick Leslie
*/

#include "Database.hh"

Database::Database(String auth, String host, String thisNode, String partnerNode) {
    _setCredentials(auth, host);
    _connectToDatabase();
    _setNodeNames(thisNode, partnerNode);
    _initDatabaseState();
} 

void Database::_setCredentials(String auth, String host) {
    _firebaseAuth = auth;
    _firebaseHost = host;
}

void Database::_connectToDatabase() {
    Firebase.begin(_firebaseHost, _firebaseAuth);
    Firebase.reconnectWiFi(true);
}

void Database::_setNodeNames(String thisNode, String partnerNode){
    _nodeName = thisNode;
    _partnerNodeName = partnerNode;
}

void Database::_initDatabaseState() {
    // Sets this node's (this lamp) state, on the database, to be false
    // Ensures potential "Stuck states" of the node will be fix on boot
    Firebase.setBool(_firebaseData, _nodeName, _swState);
}

void Database::setDatabaseRecord(bool state) {
    Firebase.setBool(_firebaseData, _nodeName, state);
}

bool Database::getDatabaseRecord() {
    Firebase.getBool(_firebaseData, _partnerNodeName);
    return _firebaseData.boolData();
}
