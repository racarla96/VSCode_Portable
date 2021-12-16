clear all;
close all;
clc;

addpath(genpath('rvctools'))

PointsCamara


SquareSize = 0.5;

x = ([0, 1, 1, 0, 0]*SquareSize) - SquareSize/2 + p(5,1);
y = ([0, 0, 1, 1, 0]*SquareSize) - SquareSize/2 + p(5,2);
x = ([0, 1, 1, 0, 0]*SquareSize) - SquareSize/2 + p(5,1);
y = ([0, 0, 1, 1, 0]*SquareSize) - SquareSize/2 + p(5,2);
figure;
hold on
plot(x, y, 'bo', 'LineWidth', 3);
plot(p(5,1), p(5,2), 'ro', 'LineWidth', 3);
plot(x,y);
axis equal

N = 2;
V = 0.1;
Ts = 0.01;
SL = 0;
for i=1:length(x)-1
D = sqrt((x(i)-x(i+1))^2+(y(i)-y(i+1))^2);
S = ceil(D/(V))*N;
[q, qd] = mtraj(@lspb, [x(i) y(i)], [x(i+1) y(i+1)], S);
plot(q(:,1), q(:,2), 'g.', 'LineWidth', 3);
end

figure; hold on;
for i=1:length(x)-1
D = sqrt((x(i)-x(i+1))^2+(y(i)-y(i+1))^2);
S = ceil(D/(V))*N;
[q, qd] = mtraj(@lspb, [x(i) y(i)], [x(i+1) y(i+1)], S);
TT = [SL*Ts:Ts:(SL+S-1)*Ts];
SL = SL + S - 1;
plot(TT, qd(:,1), 'r');
plot(TT, qd(:,2), 'b');
end

