#include<iostream>
#include <windows.h>//Windows timing
#include <conio.h>//keyboard input,ancient and non-portable:-
#include <cstdint>
#include<memory>
#include<vector>
#include<fstream>
#include <unistd.h>

#define BO 0b0000000000000001
#define BI 0b0000000000000010
#define AO 0b0000000000000100
#define AI 0b0000000000001000
#define RO 0b0000000000010000
#define RI 0b0000000000100000
#define MI 0b0000000001000000
#define HI 0b0000000010000000
#define EOFI 0b0000000100000000
#define ES 0b0000001000000000
#define EC 0b0000010000000000
#define IC 0b0000100000000000
#define II 0b0001000000000000
#define CEME 0b0010000000000000
#define CO 0b0100000000000000
#define CI 0b1000000000000000
#define ACTLOW 0b110110101111111 //1.Control signal is active low
using namespace std;

class component{
	public:
		virtual void Reset(){		//virtual suggests that the function will be overwritten in derived classes
	}
	virtual void FallingEdge(){
	}
	virtual void BeingLow(){
	}
	virtual void GettingHigh(){
	}
	virtual void RisingEdge(){
	}
	virtual void BeingHigh(){
	}
};
	
class Register:public component{ //Modeled after 74LS161/74HC161 (4-bit counter) bzw.74HC173(4-bit register 
private:
	uint16_t mStore;//internal storage
	uint8_t mPortlines;
	uint16_t mCtrllines; //Reference to IO lines the component is connected to
	uint16_t mCountMask,mInMask,mOutMask,mHiMask;
public:
Register(uint8_t portlines, uint16_t ctrl, uint16_t inmask, uint16_t outmask, uint16_t countmask, uint16_t himask)
    : mPortlines(portlines), mCtrllines(ctrl), mInMask(inmask), mOutMask(outmask), mCountMask(countmask), mHiMask(himask) {}
    
void BeingLow(){	
	if (mCtrllines & mHiMask)
	mPortlines=mStore>>8;
	else
	mPortlines=mStore& 0x00ff; 
	}
	void RisingEdge(){
		if (mCtrllines & mInMask){
			if (mCtrllines & mHiMask )
			mStore=(mStore & 0x00ff)|mPortlines<<8;
			else 
			mStore=(mStore & 0xff00)|mPortlines;
		}
		else if((mCtrllines &mCountMask)||mCountMask==0xffff)
		mStore++;
	}
	uint16_t Get(){
		return mStore;	//Direct access used by ALU and RAM
	}

};

class Adder:public component{
	private:
		uint8_t mPortlines;//Reference to IO lines the component is connected to
		uint16_t mCtrllines; //Reference to control word
		uint16_t mCinMask,mInvMask,mOutMask;
		shared_ptr<Register> mRegA,mRegB;
		uint8_t mFlaglines;//Connection to flag lines
	public:
		Adder(uint8_t &port, uint16_t &ctrl, uint16_t &outmask, uint16_t &invmask, uint16_t &cinmask, shared_ptr<Register> a, shared_ptr<Register> b, uint8_t &flaglines)
    : mPortlines(port), mCtrllines(ctrl), mOutMask(outmask), mInvMask(invmask), mCinMask(cinmask), mRegA(a), mRegB(b), mFlaglines(flaglines) {}
		
		void BeingLow(){
			int a,b;	//A and B registers
			a=int(mRegA->Get()) & 0xff;
			if (mCtrllines & mInvMask)
			b=int (~(mRegB->Get()) & 0xff);
			else
			b=int((mRegB->Get()) & 0xff);
			
			int result=a+b+bool(mCtrllines &mCinMask);	//Add operation with carry-in
			//zero flag
			if((result &0xff)==0){
				mFlaglines|=1;
			}
			else{
					mFlaglines &=~1;
			}
			//carry flag
			if(result >0xff)
				mFlaglines|=2;
			else
			mFlaglines &=~2;
			//negative flag
			if(result &0x80) 
				mFlaglines|=4;
			else
			mFlaglines &=~4;
			if(mCtrllines &mOutMask)
			mPortlines=uint8_t(result);		//output result if EO is active
		}
		
		//74HC283 works asynchroneously, use "EO|AI,EO|RI" instead of "EO|AI|RI"
		void GettingHigh(){
			BeingLow();
		}
		
};

class memory:public component{
	private:
    	uint8_t mPortlines;//Reference to IO lines the component is connected to
		uint16_t mCtrllines; //Reference to control word
		uint16_t mCinMask,mInMask,mOutMask;
		shared_ptr<Register> mMar;
		uint8_t mStore[32768];//Connection to flag lines
		string &mInBuf;//Reference to cpu input buffer
		
	public:
		memory(uint8_t &port, uint16_t &ctrl, uint16_t &inmask, uint16_t &outmask, shared_ptr<Register> mar, string &inbuf)
    : mPortlines(port), mCtrllines(ctrl), mInMask(inmask), mOutMask(outmask), mMar(mar), mInBuf(inbuf) {
        std::ifstream rom;
        rom.open("ROM.bin", std::ios::binary | std::ios::in);
        if (rom.is_open()) {
            rom.read((char*)mStore, 0x2000);
        }
    }
		
		void BeingLow(){
			if (mCtrllines &mOutMask){
				if(mMar->Get() & 0x8000){//terminal -> port
					if(mInBuf.size()>0){
						mPortlines=mInBuf[0];
						mInBuf=mInBuf.substr(1);
					}else
					mPortlines=0;
				}else
				mPortlines=mStore[mMar->Get()];//RAM -> port
			}
		}
		
		void BeingHigh(){
			if (mCtrllines &mInMask){
				if((mMar->Get() & 0x8000)==0x8000 && mPortlines!=0)
				cout<<mPortlines;
				else if(mMar->Get()>=0x2000)
				mStore[mMar->Get()]=mPortlines;
			}
		}
};

class control:public component{
	private:
		uint8_t mPortlines;//Reference to IO lines the component is connected to
		uint16_t mCtrllines; //Reference to control word
		shared_ptr<Register> mRegInstr,mRegFlags,mRegSteps;
		uint16_t mMicroCode[8192];
	
	public:
		control(uint8_t &port, uint16_t &ctrl, shared_ptr<Register> ireg, shared_ptr<Register> freg, shared_ptr<Register> sreg)
    : mPortlines(port), mCtrllines(ctrl), mRegInstr(ireg), mRegFlags(freg), mRegSteps(sreg) {
		std::ifstream lsb("CTRL_LSB.bin",std::ios::binary|std::ios::in),msb("CTRL_MSB.bin",std::ios::binary|std::ios::in);
		if(lsb.is_open() && msb.is_open()){
			for(int i=0;i<0x2000;i++){
				lsb.read((char*)& mMicroCode[i]+0,1);
				msb.read((char*)& mMicroCode[i]+1,1);
				mMicroCode[i]^=ACTLOW;
			}
		}
	}
		
		void FallingEdge() {	//Set control lines according to instruction,flags and step
		mCtrllines=mMicroCode[(mRegFlags->Get()<<10)|(mRegInstr->Get()<<4)|(mRegSteps->Get() &0b1111)];
		if(mCtrllines & IC){	//immediate asynchroneous reset
			mRegSteps->Reset();
			mCtrllines=mMicroCode[(mRegFlags->Get()<<10)|(mRegInstr->Get()<<4)|(mRegSteps->Get() &0b1111)];
		}
		if(mCtrllines & HI)
		mPortlines=0x7f;
		else mPortlines=0xff;
	}
};

class computer{
	protected:
		string mInput;
		float mSimTime;
		uint32_t mLastTicks;
		vector<shared_ptr<component>>mComponents;
		uint8_t mRegFlags;
		uint8_t mBuslines;
		uint16_t mCtrllines;
		
	public:
		computer(){
			shared_ptr<Register> areg=make_shared<Register>(mBuslines,mCtrllines,AI,AO,0,0);
			shared_ptr<Register> breg=make_shared<Register>(mBuslines,mCtrllines,BI,BO,0,0);
			shared_ptr<Register> ireg=make_shared<Register>(mBuslines,mCtrllines,II,0,0,0);
			shared_ptr<Register> freg=make_shared<Register>(mBuslines,mCtrllines,EOFI,0,0,0);
			shared_ptr<Register> sreg=make_shared<Register>(mBuslines,mCtrllines,0,0,0,0xffff);
			shared_ptr<Register> pc=make_shared<Register>(mBuslines,mCtrllines,CI,CO,CEME,HI);
			shared_ptr<Register> mar=make_shared<Register>(mBuslines,mCtrllines,MI,0,CEME,HI);
			
			shared_ptr<control> ctrl = make_shared<control>(mBuslines, mCtrllines, ireg, freg, sreg);
			shared_ptr<Adder> alu=make_shared<Adder>(mBuslines,mCtrllines,EOFI,ES,EC,areg,breg,mRegFlags);
			shared_ptr<memory> ram = make_shared<memory>(mBuslines, mCtrllines, RI, RO, mar, mInput);
			
			mComponents.emplace_back(areg);
			mComponents.emplace_back(breg);
			mComponents.emplace_back(ireg);
			mComponents.emplace_back(freg);
			mComponents.emplace_back(sreg);
			mComponents.emplace_back(pc);
			mComponents.emplace_back(mar);
			mComponents.emplace_back(ctrl);
			mComponents.emplace_back(alu);
			mComponents.emplace_back(ram);
			Reset();
	}
	void Reset(){
		for(auto & c:mComponents)c->Reset();
		mInput="";
		mLastTicks=GetTickCount();
	}
	void Update(){
		uint32_t nowticks=GetTickCount();
		mSimTime+=(nowticks-mLastTicks)*0.001f;
		mLastTicks=nowticks;
		
		while(mSimTime>1.0f/1843200.0f){
			//Update all components here over the clock cycle
			for(auto & c:mComponents)c->FallingEdge();
			for(auto & c:mComponents)c->RisingEdge();
			for(auto & c:mComponents)c->BeingHigh();
			for(auto & c:mComponents)c->BeingLow();
			for(auto & c:mComponents)c->GettingHigh();
			mSimTime-=1.0f/1843200.0f; 
		}
	}
	void Input(char s){
		mInput+=s;
	}
};

int main(){
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE),0b111);//Enable ANSI control sequence in windows console
	
	computer cpu;
	bool running=true;
	while(running){
		while(kbhit()){
			static char lastch=0;//read-in of character code
		char ch=getch();
		switch(lastch){
				case -32:
					switch(ch){
						case 79:
							running=false;
							break;
					    case 71:
					    	cpu.Reset();//Position1=reset
					    	break;
					    default:break;
						}
						break;
	            default:
	            	switch(ch){			//expecting 'single key'
	            		case -32:break; //move to special key mode
	            		case 13:
							cpu.Input('\n');
							break;
	            		default:
							cpu.Input(ch);
							break;
					}
					break;
				}
				lastch=ch;
		}
		cpu.Update();
		sleep(10);
	}
	return 0;
}
