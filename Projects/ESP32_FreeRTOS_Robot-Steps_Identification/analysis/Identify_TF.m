% load('experimento_central_difference.mat')
load('experimento_central_difference_F.mat')

% Los datos de los ficheros estan con el valor de salida invertido
% Para la versión final del código estará modificado
Ts = 0.01;
M1 = iddata(-E.out_W_M1, E.in_V_M1, Ts);
M2 = iddata(-E.out_W_M2, E.in_V_M2, Ts); 
M3 = iddata(-E.out_W_M3, E.in_V_M3, Ts); 
M4 = iddata(-E.out_W_M4, E.in_V_M4, Ts);

np = 1;
sys_M1 = tfest(M1,np);
sys_M2 = tfest(M2,np);
sys_M3 = tfest(M3,np);
sys_M4 = tfest(M4,np);

t = [0:Ts:(length(E.in_V_M1)-1)*Ts];

subplot(4, 1, 1);
lsim(sys_M1, E.in_V_M1, t); % in_V_M1 - The same input for 4 motors
hold on;
plot(t, -E.out_W_M1);

subplot(4, 1, 2);
lsim(sys_M2, E.in_V_M1, t);
hold on;
plot(t, -E.out_W_M2);

subplot(4, 1, 3);
lsim(sys_M3, E.in_V_M1, t);
hold on;
plot(t, -E.out_W_M3);

subplot(4, 1, 4);
lsim(sys_M4, E.in_V_M1, t);
hold on;
plot(t, -E.out_W_M4);

% Identified with systemIdentification
% load('identified_TF_Unfiltered.mat')

figure;
lsim(tf1, E.in_V_M1, t);
hold on;
plot(t, -E.out_W_M1);
% plot(t, -E.out_W_M2);
% plot(t, -E.out_W_M3);
% plot(t, -E.out_W_M4);

sys_M1
sys_M2
sys_M3
sys_M4
tf1

