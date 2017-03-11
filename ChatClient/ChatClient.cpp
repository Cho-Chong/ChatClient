//
//  ChatClient.cpp
//  ChatClient
//
//  Created by Cho Chong on 2/7/16.
//  Copyright © 2016 Cho Chong. All rights reserved.
//

#include "ChatClient.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "PacketWrapper.hpp"

#define SERVER_PORT ("3501")
#define HOST_NAME   ("127.0.0.1")
#define MAXDATASIZE (100)

ChatClient::ChatClient(ChatLib::Interface::IIODevice* iodevice)
{
    StreamPacketDriver = new ChatLib::PacketDriver(iodevice);
}

ChatClient::~ChatClient()
{
    
}

void* ChatClient::get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int ChatClient::Connect()
{
    int sockfd = 0;
    struct addrinfo hints, *servinfo, *cur_sock;
    int rv;
    char ip[INET6_ADDRSTRLEN];
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if( (rv = getaddrinfo(HOST_NAME, SERVER_PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 0;
    }
    else
    {
        for(cur_sock = servinfo; cur_sock != NULL; cur_sock = cur_sock->ai_next)
        {
            if( (sockfd = socket(cur_sock->ai_family, cur_sock->ai_socktype, cur_sock->ai_protocol)) == -1)
            {
                perror("client: socket");
                continue;
            }
            
            if(connect(sockfd, cur_sock->ai_addr, cur_sock->ai_addrlen) == -1)
            {
                close(sockfd);
                perror("client; connect");
                continue;
            }
            
            break;
        }
        
        if(cur_sock == NULL)
        {
            fprintf(stderr, "client: failed to connect\n");
            return 0;
        }
        
        inet_ntop(cur_sock->ai_family, get_in_addr((struct sockaddr*) cur_sock->ai_addr), ip, sizeof(ip));
        
        
        printf("client; connecting to %s\n", ip);
        this->socketfd = sockfd;
        freeaddrinfo(servinfo);

        return 1;
    }
}

void ChatClient::Close()
{
    printf("client: closing socket\n");
    close(this->socketfd);
    this->socketfd = -1;
}

void ChatClient::QueueTransmit(const Model::Message& message)
{
    ChatLib::PACKET_T packet;
    ChatLib::PacketWrapper::Wrap((char*) &message, sizeof(message), packet, message.GetType(), ChatLib::NODE_SERVER, ChatLib::NODE_CLIENT);
    this->StreamPacketDriver->QueuePacket(this->socketfd, packet);
}

void ChatClient::Transmit()
{
    StreamPacketDriver->SendPackets(this->socketfd);
}

void ChatClient::Receive()
{
    StreamPacketDriver->RecvPacket(this->socketfd);
}

//HACK: should not handle casting packets here
void ChatClient::Dequeue(const Model::User& user)
{
    ChatLib::PACKET_T packet;
    packet.header.length = 0;
    
    do
    {
        //TODO: need to somehow map an incoming message to a user
        // option 1: ask the server for a user list
        // option 2: have the user name in the message
        // option 3: ask the server for the user
        StreamPacketDriver->DequeuePacket(packet);
        
        if(packet.header.length > 0 && packet.header.dest == ChatLib::NODE_CLIENT && packet.header.type == ChatLib::TYPE_MESSAGE)
        {
            Model::Message* message;
            
            message = (Model::Message*) (&packet.body);
            
            if(message->GetUserId() != user.GetId())
            {
                printf("%d    %s: %s\n", message->GetTime(), user.Name, message->Entry);
            }
        }
    }
    while( packet.header.length > 0);
}