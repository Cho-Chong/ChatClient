//
//  main.cpp
//  ChatClient
//
//  Created by Cho Chong on 2/7/16.
//  Copyright © 2016 Cho Chong. All rights reserved.
//

#include "ChatClient.hpp"
#include <pthread.h>
#include <string.h>
#include <iostream>
#include "Record.hpp"
#include "Threads.h"

#define USER_NAME ("Joe")

static bool quit = false;
static ChatClient client;
static Model::User User;


void* TransmitThread(void *thread_id)
{
    do
    {
        client.Transmit();
        usleep(50000);
    }
    while(!quit);
    
    pthread_exit(NULL);
}

void* UserInputThread(void *thread_id)
{
    std::string input;
    Model::Message message;
    message.SetUserId(User.GetId());
    
    do
    {
        std::cin >> message.Entry;
        //TODO: get timestamp
        quit = (strcmp(message.Entry, "exit") == 0);
        
        if(!quit)
        {
            client.QueueTransmit(message);
        }
    }
    while(!quit);
    
    pthread_exit(NULL);
}

void* ReceiveThread(void *thread_id)
{
    do
    {
        client.Receive();
    }
    while(!quit);
    
    pthread_exit(NULL);
}

void* Display(void * thread_id)
{
    do
    {
        client.Dequeue(User);
        usleep(300000);
    }
    while(!quit);
    
    pthread_exit(NULL);
}

void CreateThreads()
{
    pthread_t threads[NUM_THREADS];
    
    if(pthread_create(&threads[THREAD_RECEIVE], NULL, ReceiveThread, 0))
    {
        printf("Client: Failed to create rx thread\n");
    }
    
    if(pthread_create(&threads[THREAD_INPUT], NULL, UserInputThread, 0))
    {
        printf("Client: Failed to create input thread\n");
    }
    
    if(pthread_create(&threads[THREAD_TRANSMIT], NULL, TransmitThread, 0))
    {
        printf("Client: Failed to create tx thread\n");
    }
    
    if(pthread_create(&threads[THREAD_DISPLAY], NULL, Display, 0))
    {
        printf("Client: Failed to create display thread\n");
    }
}

int main(int argc, const char * argv[])
{
    //HACK
    std::cout << "Enter User Id: ";
    std::cin >> User.Name;
    User.SetId(User.Name[0]);
    
    if(client.Connect())
    {
        CreateThreads();
    }
    
    while(!quit)
    {
        sleep(1);
    }
    
    client.Close();
    // need to spawn two threads, one to keep receiving packets, one to constantly send
    
    pthread_exit(NULL);
}
