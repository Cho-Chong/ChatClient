//
//  ChatClient.hpp
//  ChatClient
//
//  Created by Cho Chong on 2/7/16.
//  Copyright © 2016 Cho Chong. All rights reserved.
//

#ifndef ChatClient_hpp
#define ChatClient_hpp

#include <stdio.h>
#include "PacketDriver.hpp"
#include "Record.hpp"

class ChatClient
{
public:
    ChatClient();
    ~ChatClient();
    
    int Connect();
    void Close();
    void QueueTransmit(const Model::Message& message);
    void Transmit();
    void Receive();
    void Dequeue(const Model::User& user);
private:
    
    void *get_in_addr(struct sockaddr *sa);
    ChatLib::PacketDriver StreamPacketDriver;
    int socketfd;
};

#endif /* ChatClient_hpp */
