clc;
clear all;
close all;

load('toCompare.mat');

fig = figure; hold on;
title("X-Y Plot");
plot(filteroutmat(:,1),filteroutmat(:,2),'o-');
plot(filteroutcpp(:,1),filteroutcpp(:,2),'.-');
legend('matlab','cpp');
xlabel("X (m)");
ylabel("Y (m)");
saveas(fig,'X-Y_Plot.png');
saveas(fig,'X-Y_Plot.fig');

T = [0:0.1:3000*0.1];

fig = figure; hold on;
title("T-X Plot");
plot(T,filteroutmat(:,1),'o-');
plot(T,filteroutcpp(:,1),'.-');
legend('matlab','cpp');
xlabel("T (s)");
ylabel("X (m)");
saveas(fig,'T-X_Plot.png');
saveas(fig,'T-X_Plot.fig');

fig = figure; hold on;
title("T-Y Plot");
plot(T,filteroutmat(:,2),'o-');
plot(T,filteroutcpp(:,2),'.-');
legend('matlab','cpp');
xlabel("T (s)");
ylabel("Y (m)");
saveas(fig,'T-Y_Plot.png');
saveas(fig,'T-Y_Plot.fig');

fig = figure; hold on;
title("T-Theta Plot");
plot(T,filteroutmat(:,3),'o-');
plot(T,filteroutcpp(:,3),'.-');
legend('matlab','cpp');
xlabel("T (s)");
ylabel("Theta (rad)");
saveas(fig,'T-Theta_Plot.png');
saveas(fig,'T-Theta_Plot.fig');

fig = figure; hold on;
title("T-Vx Plot");
plot(T,filteroutmat(:,4),'o-');
plot(T,filteroutcpp(:,4),'.-');
legend('matlab','cpp');
xlabel("T (s)");
ylabel("Vy (m/s)");
saveas(fig,'T-Vx_Plot.png');
saveas(fig,'T-Vx_Plot.fig');

fig = figure; hold on;
title("T-Vy Plot");
plot(T,filteroutmat(:,5),'o-');
plot(T,filteroutcpp(:,5),'.-');
legend('matlab','cpp');
xlabel("T (s)");
ylabel("Vy (m/s)");
saveas(fig,'T-Vy_Plot.png');
saveas(fig,'T-Vy_Plot.fig');

fig = figure; hold on;
title("T-Wz Plot");
plot(T,filteroutmat(:,6),'o-');
plot(T,filteroutcpp(:,6),'.-');
legend('matlab','cpp');
xlabel("T (s)");
ylabel("Wz (rad/s)");
saveas(fig,'T-Wz_Plot.png');
saveas(fig,'T-Wz_Plot.fig');
