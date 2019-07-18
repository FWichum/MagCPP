#include "magstim.h"

MagStim::MagStim(std::string port)
{
    this->connect(port);
}

bool MagStim::connect(std::string port)
{

    //FIXME: get portnumber from portlist
    bool connected=false;
    cp_num=0;
    connected=RS232_OpenComport(cp_num,bdrate,mode);
    //Check if connection was established
    if(connected){
        return true;
    }
    else {
        return false;
    }
}

void MagStim::disconnect()
{
    RS232_CloseComport(cp_num);
}

bool MagStim::encode_command(uint8_t *destination, uint8_t *data)
{
    //Muss noch angepasst werden
    destination[4]=*data;
        destination[5]=*++data;
        destination[6]=*++data;

        std::stringstream test;
        std::string hexstring;
        test<<std::setbase(16);
        test<<std::uppercase;
        test<<(0xff&(destination[0]+destination[1]+
                destination[2]+destination[3]+
                destination[4]+destination[5]+
                destination[6]+destination[7]+
                destination[8]));
        //std::cout<<test.str();
        hexstring+=test.str();
        destination[9]= hexstring[0];
        destination[10]= hexstring[1];
}

bool MagStim::get_status()
{
    RS232_SendBuf(cp_num,stat_command,10);
}
