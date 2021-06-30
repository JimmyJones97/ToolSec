#include <stdio.h>
#include <unistd.h>
#include <string>
#include <locale.h>
#include <wchar.h>

#include "FIFOReader.h"
#include "proto/addressbook.pb.h"
#include "proto/wallhackdata.pb.h"

int main(int argc, char* argv[]){
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    tutorial::AddressBook address_book;
    tutorial::Person *person = address_book.add_people();
    person->set_id(10001);
    person->set_email(std::string("abc@gmail.com"));
    person->set_name("Alex Mahong");
    tutorial::Person::PhoneNumber* phone_number1 = person->add_phones();
    phone_number1->set_number("15898752233");
    phone_number1->set_type(tutorial::Person::MOBILE);
    tutorial::Person::PhoneNumber* phone_number2 = person->add_phones();
    phone_number2->set_number("010-6668888");
    phone_number2->set_type(tutorial::Person::WORK);

    std::string output;
    address_book.SerializeToString(&output);
    const char *p = output.c_str();
    for(int i=0; i<output.size(); i++){
        printf("[%d] %02x %c\n", i, *p, *p);
        p++;
    }
    

    for(int i=0; i<address_book.people_size(); i++){
        const tutorial::Person& person = address_book.people(i);
        printf("people [%d] id:%d, name:%s, email:%s\n", i, person.id(), person.name().c_str(), person.email().c_str());
        for(int j=0; j<person.phones_size();j++){
            const tutorial::Person::PhoneNumber& phone_number = person.phones(j);
            printf("  phones[%d] type:%d number:%s\n", j, phone_number.type(), phone_number.number().c_str());
        }
    }

    FIFOReader fifo_reader("/data/local/tmp/test_fifo");
    if(0 != fifo_reader.init_fifo()){
        printf("init reader fifo failed\n");
        return -1;
    }
    printf("init reader fifo ok\n");
    while (true)
    {
        printf("read data...\n");
        wallhack::PlayersPerFrame players_per_frame;
        players_per_frame.ParseFromFileDescriptor(fifo_reader.get_fd());
        for(int i=0; i<players_per_frame.player_size(); i++){
            const wallhack::Player& player = players_per_frame.player(i);
            wchar_t name_wc[256] = {0};
            const std::string &name_str = player.player_name();
            int PlayerName_Count = name_str.size() / 2;
            uint16_t *name_16 = (uint16_t*)name_str.c_str();
            for(int j=0; j<PlayerName_Count; j++){
                name_wc[j] = *name_16;
                //printf("%04x %08x\n", *name_16, name_wc[j]);
                name_16++;
            }
            setlocale(LC_ALL, "");
            printf("player[%d] %ls screen_pos:(%f,%f) health:%f\n", 
                i, 
                name_wc, 
                player.screen_pos().x(), 
                player.screen_pos().y(),
                player.health());
        }

        
        // if(0 != fifo_reader.read_data()){
        //     printf("read data failed\n");
        //     break;
        // }
        sleep(5);
    }

    printf("while break\n");
    // unlink("/data/local/tmp/test_fifo");

    return 0;
    
}