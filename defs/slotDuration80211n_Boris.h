function SlotDuration80211n()


% Minimum Performance (No Aggregation)
M=1;
subcarriers = 48;
Mod = 1;
Coding = 1/2;
L= 12000;
Nag = 1;

TxD=TxServiceTime(M,L,Nag,Coding,Mod,subcarriers,M);

% Maximum Performance (No Aggregation)
M=4;
subcarriers = 108;
Mod = 6;
Coding = 5/6;
L= 12000;
Nag = 1;

TxD=TxServiceTime(M,L,Nag,Coding,Mod,subcarriers,M);

% Minimum Performance (Aggregation)
M=1;
subcarriers = 48;
Mod = 1;
Coding = 1/2;
L= 12000;
Nag = 64;

TxD=TxServiceTime(M,L,Nag,Coding,Mod,subcarriers,M);

% Maximum Performance (Aggregation)
M=4;
subcarriers = 108;
Mod = 6;
Coding = 5/6;
L= 12000;
Nag = 64;

TxD=TxServiceTime(M,L,Nag,Coding,Mod,subcarriers,M);



function [TxD,S] = TxServiceTime(s,L,N,Coding,Mod,subcarriers,M)
% s: Number of Spatial Streams (1,2,3,4)
% L: Packet Length
% N: Number of packets aggregated together
% Coding: Channel Coding (1/2, 2/3, ... )
% Modulation: Bits per symbol (BPSK = 1, QPSK = 2, etc.)
% Subcarriers: 20 MHz = 48, 40 MHz = 108
% M: Number of antennas >= s



    SIFS = 16E-6;
    DIFS = 34E-6;
    SLOT = 9E-6;
    
	Ts=4E-6; % 800 ns guard interval
    
    DataRate = (s*subcarriers*Mod*Coding)/Ts;    
    
    % Mided MODE 
	PHY_h_MM = 8E-6 + 8E-6 + 4E-6 + 8E-6 + 4E-6 + M*4E-6;

    % Green field MODE
	PHY_h_GF = 8E-6 + 8E-6 + 8E-6 +  M*4E-6;
        
    
    PHY_h=PHY_h_GF;
    
    % MAC header
	MAC_h=36*8;
	FCS=4*8;

    % Aggregation Parameters (A-MPDU)
	MPDU_Del=4*8;     
    
	BACK=32*8; % Compressed Bitmap

	MPDU=MAC_h+L+FCS;

	ServiceField=16; 
    Nes = 1; % Convolutional Encoding Branches
	Tail_bits=6*Nes;

	PSDU = -1;
    
	if(N==1) PSDU = ServiceField + MPDU + Tail_bits;
	else PSDU = ServiceField + N*(MPDU_Del+MPDU) + Tail_bits;
    end
        
	TxPSDU = PHY_h + ceil(PSDU/(subcarriers*Mod*Coding))*Ts;

	PSDU_BAK = ServiceField + BACK +Tail_bits;

	TxBAK = PHY_h+ceil(PSDU_BAK/(s*subcarriers*Mod*Coding))*Ts;

	TxD = TxPSDU+SIFS+TxBAK + DIFS + SLOT;
    	         
    disp('Slot Duration [seconds] | Throughput (continuous tx) [Mbps] | Data Rate [Mbps] | Eficiency (%)');
	disp([TxD N*(L/TxD)/1E6 DataRate/1E6 (N*(L/TxD)/1E6)/(DataRate/1E6)*100]);
    
    