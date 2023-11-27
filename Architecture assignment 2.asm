#Authors:BinaryLovers(Mokhosi Katse,Poelo Koatsa,Taelo Nkoho,Mamothabi Pholosi,Khoabane Ntai,Nnile Pobetsi and Morapeli Tau)
#Date:November 20, 2023
#===============================================================================================================================================
#Functional description:This MIPS Assembly program simulates the famous Machobane Farming System(MFS) in Lesotho
#1.Main passes soil fertility content as a parameter to a function isFertile
#2.Function isFertile returns True if the soil is fertile and false otherwise,then prints appropriate msg
#3.Main calls a non-parametric function relayplanting
#4.Function relayplanting prompts the user to enter primary and relay crops,increments integer current month(initialized globally as 1),then passes entered crops as parameters to a funtion intercropping
#5.Function intercropping extracts the parameters and shows their intercroped model at appropriate periods(seasons)
#6.Main passes soil moisture as a parameter to a function isMoist
#7.Function isMoist returns True,prints a msg that irrigation is needed and False,then prints appropriate msg otherwise
#8.Main passes soil fertility content as a parameter to a function FindCropYield
#9.Function FindCropYield projects/estimates the potential Total yield using the rule "potentialyield = 10tonnes *fertility content"
#10.Main exits
#pseudo code description using C++
#================================================================================================================================================
#int currmonth=1;//global variable that initializes current month as 1(January)
#bool isFertile(double fertilityContent){
#	bool fertility=false;
#	if(fertilityContent>=65)
#	{
#		fertility=true;
#		cout<<"No need to fertilize the soil!"<<endl;
#	}
#	else{
#		cout<<"Fertilize the soil!"<<endl;;
#	}
#	return isFertile;
#}

#void intercropping(string crop1,string crop2){
#	cout<<crop1<<":-----------"<<endl;
#	cout<<crop2<<":***********"<<endl;
#	cout<<crop1<<":-----------"<<endl;
#	cout<<crop2<<":***********"<<endl;
#}

#void relayplanting()//Managing relay planting with intercropping
#{
#	string primarycrop,primaryIntercrop,relaycrop,relayIntercrop;//Define primary crops and relay crops
#	cout<<"Enter primary crop:";
#	cin>>primarycrop;
#	cout<<"Enter another primary crop(intercrop):";
#	cin>>primaryIntercrop;
#	cout<<"Enter relay crop:";
#	cin>>relaycrop;
#	cout<<"Enter another relay crop(intercrop):";
#	cin>>relayIntercrop;
#	while(currmonth<=12){
#		if(currmonth>=4&&currmonth<=5)//Plant the primary intercrop simultaneously with the primary crop at the beginning of winter
#		{	
#		cout<<"primary crops planted!"<<endl;
#		intercropping(primarycrop,primaryIntercrop);
#		}
#		if(currmonth>=8&&currmonth<=10)//Plant the relay intercrop simultaneously with the relay crop at the beginning of summer
#		{
#		cout<<"relay crops planted under the shade of maturing primary crops!"<<endl;
#		    intercropping(relaycrop,relayIntercrop);
#		}
#	currmonth++;
#	}
#}

#bool isMoist(double moisturecontent){
#	bool moisture=false;
#	if(moisturecontent>=65)
#	{
#		moisture=true;
#		cout<<"No need to irrigate the soil!"<<endl;
#	}
#	else{
#		cout<<"Irrigate the soil!"<<endl;
#	}
#	return isMoist;
#};
#void FindCropYield(double fertilityContent){
#	double yieldPercrop=10;
#	if (isFertile(fertilityContent)){
#		double Cropyield=yieldPercrop*fertilityContent;
#		cout<<"The expected yield is:"<<Cropyield<<endl;
#	}
#	else{
#		double Cropyield=yieldPercrop*fertilityContent;
#		cout<<"The expected yield is less than:"<<Cropyield<<"Quite low!"<<endl;
#	}
#}

#int main(){
#	isFertile(22);
#	//intercropping("lihoete","tamati");
#	relayplanting();
#	FindCropYield(70);
#	isMoist(12);
#	return 0;
#}
#==================================================================================================================================================
  .data
  msg00: .asciiz "CHECK THE SOIL FERTILITY CONTENT BEFORE  PLANTING\n\n"
  if: .asciiz "No need to fertilize the soil\n\n" #msg to print if the soil is fertile 
  else: .asciiz "Fertilize the soil!\n\n"          #msg to print if the soil needs fertilising
  if3: .asciiz "No need to irrigate the soil!\n"
  else1: .asciiz "Irrigate the soil!\n"
  
  #Assumption:We start planting in winter, hence the primary crops will always be winter crops and relay crops will always be summer crops
  msg0: .asciiz "MANAGING RELAY PLANTING WITH INTERCROPPING\n\n"
  if1: .asciiz "HERE ARE PRIMARY CROPS INTERCROPPED:\n"       #msg to print when primary crops have just been planted
  if2: .asciiz "HERE ARE RELAY CROPS INTERCROPPED UNDER THE SHADE OF MATURING PRIMARY CROPS:\n"
  prompt: .asciiz "what is the sensored fertility content of the soil?\n"
  prompt2: .asciiz "What is the moisture level value as sensored in the soil?\n"
  msg: .asciiz "You have entered:\n"
  msg1: .asciiz "\n"
  msg2: .asciiz "We have reached the end of the year!\n"
  msg3: .asciiz "The expected(estimated) yield is:\n"
  ms4: .asciiz "Quite low!\n"
  primarycrop: .asciiz "------------------------------------------\n\n"
  primaryIntercrop: .asciiz "#####################\n"
  relaycrop: .asciiz "********************************************\n"
  relayIntercrop: .asciiz "@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n"
  buffer: .space 1024
  crop1: .asciiz "Enter primary crop\n"
  crop2: .asciiz "Enter primaryIntercrop\n"
  crop3: .asciiz "Enter relaycrop\n"
  crop4: .asciiz "Enter relayIntercrop crop\n"
  .text
  
main:
#check the soil fertility content before  planting
	li $v0,4       #Display label
	la $a0,msg00   
	syscall
	
	li $t0,65  #store the value of the minimum fertility Content that the soil must have in reg t0
	
	#prompt user to enter the soil fertility content value as sensored
	li $v0,4
	la $a0,prompt
	syscall
	
	li $v0, 5 #get the soil fertility value from user
	syscall
	move $t1,$v0 #copy the entered value into register $t1 to avoid overwriting it when we do another systemcall
	
	
	li $v0,4
	la $a0,msg
	syscall
	
	li $v0,1
	move $a0,$t1
	syscall
	
	li $t2,10 #store yieldPercrop value into register t2
	
	li $v0,4
	la $a0,msg1
	syscall
	
	ble $t1,$t0,fertilize
	li $v0,4
	la $a0,if
	syscall
	
	li $v0,4
	la $a0,msg3
	syscall
	
	li $v0,1
	mult $t2,$t1 
	mflo $t3
	move $a0,$t3
	syscall
	
	li $v0,4
	la $a0,msg1
	syscall
	
	j end   #jump to the end of the isFertile function code
	syscall
	
fertilize:
	li $v0,4
	la $a0,msg3
	syscall
	
	li $v0,1
	mult $t2,$t1 
	mflo $t3
	move $a0,$t3
	syscall
	
	li $v0,4
	la $a0,msg1
	syscall
	
	li $v0,4
	la $a0,ms4
	syscall
	
	li $v0,4
	la $a0,else
	syscall
#end of isFertile function
end:
	j relayplanting

#Managing relay planting with intercropping
relayplanting:
li $v0,4
			la $a0,msg0
			syscall
			
#prompt user to enter primarycrop name
			li $v0,4  
			la $a0,crop1
			syscall

#read primarycrop from user
			li $v0,8   
			la $a0,buffer
			syscall
			
			li $v0,4
			la $a0,msg
			syscall
			
			li $v0,4
			la $a0,buffer
			syscall
	
#prompt user to enter primaryIntercrop name
			li $v0,4
			la $a0,crop2
			syscall
			
#read primaryIntercrop from user
			li $v0,8
			la $a0,buffer
			syscall
			
			li $v0,4
			la $a0,msg
			syscall
			
			li $v0,4
			la $a0,buffer
			syscall
			
#prompt user to enter relaycrop name
			li $v0,4
			la $a0,crop3
			syscall
			
#read relaycrop from user
			li $v0,8
			la $a0,buffer
			syscall
			
			li $v0,4
			la $a0,msg
			syscall
			
			li $v0,4
			la $a0,buffer
			syscall
			
#prompt user to enter relayIntercrop name
			li $v0,4
			la $a0,crop4
			syscall
			
#read relayIntercrop from user
			li $v0,8
			la $a0,buffer
			syscall
			
			li $v0,4
			la $a0,msg
			syscall
			
			li $v0,4
			la $a0,buffer
			syscall

#Keep incrementing currMonth by 1 as we loop through planting primary crops and relay crops times(months) to the end of the year
while:
#initialize the first month 1(January) and the last month 12(December) since there can only be 12 months in a year
li $t4,1 #currMonth
li $t5,12

loop:
bge $t4,$t5,endloop
bne $t4,4,endl

li $v0,4
la $a0,if1
syscall

li $v0,4
la $a0,primarycrop
syscall

li $v0,4
la $a0,primaryIntercrop
syscall
addi $t4,1
j loop

endl:
bne $t4,5,endlif
li $v0,4
la $a0,primarycrop
syscall

li $v0,4
la $a0,primaryIntercrop
syscall
addi $t4,1
j loop

endlif:
bne $t4,8,endliff
li $v0,4
la $a0,if2
syscall

li $v0,4
la $a0,relaycrop
syscall

li $v0,4
la $a0,relayIntercrop
syscall
addi $t4,1
j loop

endliff:
bne $t4,9,endlifff
li $v0,4
la $a0,relaycrop
syscall

li $v0,4
la $a0,relayIntercrop
syscall
addi $t4,1
j loop

endlifff:
bne $t4,10,endlifffelse
li $v0,4
la $a0,relaycrop
syscall

li $v0,4
la $a0,relayIntercrop
syscall
addi $t4,1
j loop

endlifffelse:
addi $t4,1
j loop

endloop:
	li $v0,4
	la $a0,msg2
	syscall
	
	j isMoist

isMoist:
	li $v0,4
	la $a0,prompt2
	syscall
	
	li $t6,65 #Stote the value of the minimum soil moisture in t6
	
	
	li $v0,5
	syscall
	move $t7,$v0
	
	li $v0,4
	la $a0,msg
	syscall
	
	li $v0,1
	move $a0,$t7
	syscall
	
	li $v0,4
	la $a0,msg1
	syscall
	
	ble $t7,$t6,irrigate
	li $v0,4
	la $a0,if3
	syscall
	
	j endline   #jump to the end of the isFertile function code
	syscall
	
irrigate:
		li $v0,4
		la $a0,else1
		syscall
#end of isFertile function
endline:

li $v0,10
syscall