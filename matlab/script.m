clc;
clear;
close all;

load('obs_sec.mat')
load('obs_click.mat')

hold on
plot(obs_sec(:,1),'r','LineWidth',2,'DisplayName','Open Image')

plot(obs_sec(:,3),'c','LineWidth',2,'DisplayName','Add Second Image')
plot(obs_sec(:,4),'m','LineWidth',2,'DisplayName','Select Annotation')
plot(obs_sec(:,5),'k','LineWidth',2,'DisplayName','Delete Image')
plot(obs_sec(:,7),'g','LineWidth',2,'DisplayName','Edit Annotation')
%plot(obs_sec(:,8),'g')
plot(obs_sec(:,9),'b','LineWidth',2,'DisplayName','Save')
set(get(gca,'XLabel'),'String','Experiment')
set(get(gca,'YLabel'),'String','Seconds')
set(gca,'XTick',[1:3])
figure
hold on
plot(obs_click(:,1),'--r','LineWidth',2,'DisplayName','Open Image')
plot(obs_click(:,3),'--c','LineWidth',2,'DisplayName','Add Second Image')
plot(obs_click(:,4),'--m','LineWidth',2,'DisplayName','Select Annotation')
%plot(obs_click(:,5),'--k','LineWidth',2)
plot(obs_click(:,7),'--g','LineWidth',2,'DisplayName','Edit Annotation')
%plot(obs_sec(:,8),'g')
plot(obs_click(:,9),'--b','LineWidth',2,'DisplayName','Save')