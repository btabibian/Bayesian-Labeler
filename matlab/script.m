clc;
clear;
close all;

load('obs_sec.mat')
load('obs_click.mat')
load('sat.mat')

hold on
plot(obs_sec(:,1),'r','LineWidth',2,'DisplayName','Open Image')
plot(obs_sec(:,3),'c','LineWidth',2,'DisplayName','Add Annotation')
plot(obs_sec(:,4),'m','LineWidth',2,'DisplayName','Select Annotation')
plot(obs_sec(:,5),'k','LineWidth',2,'DisplayName','Delete Image')
plot(obs_sec(:,7),'g','LineWidth',2,'DisplayName','Edit Annotation')
plot(obs_sec(:,9),'b','LineWidth',2,'DisplayName','Save')
set(get(gca,'XLabel'),'String','Experiment')
set(get(gca,'YLabel'),'String','Seconds')
set(gca,'XTick',[1:3])
figure
hold on
plot(obs_click(:,1),'--r','LineWidth',2,'DisplayName','Open Image')
plot(obs_click(:,3),'--c','LineWidth',2,'DisplayName','Add Second Image')
plot(obs_click(:,4),'--m','LineWidth',2,'DisplayName','Select Annotation')
plot(obs_click(:,7),'--g','LineWidth',2,'DisplayName','Edit Annotation')
plot(obs_click(:,9),'--b','LineWidth',2,'DisplayName','Save')
figure
hold on
ave=mean(obs_sec);
ave_sat=[ave; sat]
set(get(gca,'XLabel'),'String','Time(s) to Complete a Task')
set(get(gca,'YLabel'),'String','User Rating')
set(gca,'XLim',[0 60])
set(gca,'YLim',[0 5])
set(gca,'XTick',[0:10:60])
set(gca,'YTick',[0:1:5])
%set(gca,'Color',[0.7,0.7,0.7])
size=6
plot(obs_sec(1,1),ave_sat(2,1),'*r','MarkerSize', size,'LineWidth', size/2,'DisplayName','Open Image');
plot(obs_sec(1,3),ave_sat(2,3),'dc','MarkerSize', size,'LineWidth', size/2,'DisplayName','Add Annotation');
plot(obs_sec(1,4),ave_sat(2,4),'sm','MarkerSize', size,'LineWidth', size/2,'DisplayName','Select Annotation');
plot(obs_sec(1,5),ave_sat(2,5),'hk','MarkerSize', size,'LineWidth', size/2,'DisplayName','Delete Image');
plot(obs_sec(1,7),ave_sat(2,7),'xg','MarkerSize', size,'LineWidth', size/2,'DisplayName','Edit Annotation');
plot(obs_sec(1,9),ave_sat(2,9),'vb','MarkerSize', size,'LineWidth', size/2,'DisplayName','Save');
