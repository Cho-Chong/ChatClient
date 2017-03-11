//
//  ChatDisplay.hpp
//  ChatClient
//
//  Created by Cho Chong on 2/12/17.
//  Copyright © 2017 Cho Chong. All rights reserved.
//
//  ChatDisplay handles how to display incoming messages and options for those message
//  i.e. translations, deletes, links, etc

#ifndef ChatDisplay_hpp
#define ChatDisplay_hpp

#include <stdio.h>
#include "Record.hpp"

class ChatDisplay
{
public:
    ChatDisplay();
    ~ChatDisplay();
    
    void DisplayMsg(const Model::Message &msg);
private:
};

#endif /* ChatDisplay_hpp */
