/*
    Database.cpp - For connecting to a Firebase Realtime database
    Created by Nick Leslie
*/

#include "Database.hh"

Database::Database(String auth, String host, String thisNode, String partnerNode) {
  _setCredentials(auth, host);
  _connectToDatabase();
  _setNodeNames(thisNode, partnerNode);
}

void Database::_setCredentials(String auth, String host) {
  _firebaseAuth = auth;
  _firebaseHost = host;
}

void Database::_connectToDatabase() {
  Firebase.begin(_firebaseHost, _firebaseAuth);
  Firebase.reconnectWiFi(true);
}

void Database::_setNodeNames(String thisNode, String partnerNode) {
  _nodeName = thisNode;
  _partnerNodeName = partnerNode;
}

void Database::resetNodeState() {
  // Sets this node's (this lamp) state, on the database, to be false
  // Ensures potential "Stuck states" of the node will be fix on boot
  
  Firebase.setBool(_firebaseData, _nodeName + _path, _swState);
  Firebase.setBool(_firebaseData, _partnerNodeName + _path, _swState);
}

bool Database::setCurrentNodeRecord(bool state) {
  return Firebase.setBool(_firebaseData, _nodeName + _path, state);
}

bool Database::getPartnerNodeRecord() {
  Firebase.getBool(_firebaseData, _partnerNodeName + _path);
  return _firebaseData.boolData();
}

int Database::getConfig() {
  Firebase.getInt(_firebaseData, "/config/polling_rate");
  return _firebaseData.intData();
}
