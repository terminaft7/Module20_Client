#include "chatpool.h"
#include <string>
#include <iostream>
#include <vector>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#define MESSAGE_BUFFER 4096 // Максимальный размер буфера для приема и передачи
#define PORT 7777 // Номер порта, который будем использовать для приема и передачи 
char buffer[MESSAGE_BUFFER];
int socket_descriptor, message_size;
struct sockaddr_in serveraddress;




using namespace std;

int chatpool::checkmap(string receiver, string nickname) {
	int v = this->chatpool_vect.size();
	int counter = 0;

	if (!this->chatpool_vect.empty()) {
		for (auto& u : this->chatpool_vect) {
	
				if (u.begin()->second == receiver + "_" + nickname || u.begin()->second == nickname + "_" + receiver) {
				   
                    return counter;
				}
            counter++;
			}
		}
	return -1;
	}

void chatpool::showmap(string receiver, string nickname) {
    int ind(0);
	for (auto& u : this->chatpool_vect) {

        if (u.begin()->second == receiver + "_" + nickname || u.begin()->second == nickname + "_" + receiver) {

            for (map<int, string>::iterator it = u.begin(); it != u.end(); ++it)
            {
                cout << it->second << endl;
                ind++;
            }
        }
	}
    if (ind == 0) { cout << "You did not chat with " << receiver << " yet! Start a conversation!" << endl; }
}

void chatpool::sendmessage(string receiver, string nickname, string message) {
    string complete_message = "from " + nickname + ": " + message;
  
    int y = this->checkmap(receiver, nickname);
    if (y == -1) {

        map <int, string> mapname;
        string ident = receiver + "_" + nickname;
        mapname.insert({ 0,ident });
        mapname.insert({ 1, complete_message });
        this->chatpool_vect.push_back(mapname);
        
    }
    else if (y > -1) {
        int size_map = this->chatpool_vect[y].size();
        this->chatpool_vect[y].insert({ size_map +1, complete_message });
    }
}


void chatpool::sendRequest(string nickname, string receiver) {
    
    cout << "SERVER IS LISTENING THROUGH THE PORT: " << PORT << " WITHIN A LOCAL SYSTEM" << endl;
    // Укажем адрес сервера
    serveraddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Зададим номер порта для соединения с сервером
    serveraddress.sin_port = htons(PORT);
    // Используем IPv4
    serveraddress.sin_family = AF_INET;
    // Создадим сокет 
    socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    // Установим соединение с сервером
    if (connect(socket_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress)) < 0) {
        cout << endl << " Something went wrong Connection Failed" << endl;
        exit(1);
    }
// Запускаем функцию обработки сообщений от клиентов и ответа на эти сообщения
    cout << "CLIENT IS ESTABLISHING A CONNECTION WITH SERVER THROUGH PORT: " << PORT << endl;
//Converting nickname from string to char   
    int k;
    for (k = 0; k < sizeof(nickname); k++) {
        buffer[k] = nickname[k];
    }

    
    sendto(socket_descriptor, buffer, MESSAGE_BUFFER, 0, nullptr, sizeof(serveraddress));

    char answer[4096];

    recvfrom(socket_descriptor, answer, sizeof(buffer), 0, nullptr, nullptr);
 
    //Converting receiver from string to char for comparison in strcmp  
    int l;
    char receiver_char [4096];
    for (l = 0; l < sizeof(receiver); l++) {
        receiver_char[l] = receiver[l];
    }
    
    if (strcmp(answer, receiver_char) == 0)
    {    
       
    while (1) {
        cout << "Enter a message you want to send to the server: " << endl;
        cin >> buffer;

        if (strcmp(buffer, "end") == 0) {
            sendto(socket_descriptor, buffer, MESSAGE_BUFFER, 0, nullptr, sizeof(serveraddress));
            cout << "Client work is done.!" << endl;
            close(socket_descriptor);
 
           return;
        }
        else {
            sendto(socket_descriptor, buffer, MESSAGE_BUFFER, 0, nullptr, sizeof(serveraddress));
            cout << "Message sent successfully to the server: " << buffer << endl;
            cout << "Waiting for the Response from Server..." << endl;
        }
        
        this->sendmessage(receiver, nickname, buffer);
        cout << "Message Received From Server: " << endl;
        recvfrom(socket_descriptor, buffer, sizeof(buffer), 0, nullptr, nullptr);
        cout << buffer << endl;
        this->sendmessage(nickname, receiver, buffer);
    }
    // закрываем сокет, завершаем соединение
    close(socket_descriptor);
    return;
    
    }
    
    if (strcmp(answer, receiver_char) != 0) {
        cout << "Sorry, the user you want to chat it with is not online. Please try later." << endl;
        sendto(socket_descriptor, "end", MESSAGE_BUFFER, 0, nullptr, sizeof(serveraddress));
        close(socket_descriptor);
        return;
    }

}
    

