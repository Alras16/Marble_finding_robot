%% Data
x = test_gamma(1,1:47);

% Average Reward
ar_1 = test_gamma(2,1:47);
ar_2 = test_gamma(4,1:47);
ar_3 = test_gamma(6,1:47);
ar_4 = test_gamma(8,1:47);
ar_5 = test_gamma(10,1:47);
ar_6 = test_gamma(12,1:47);
ar_7 = test_gamma(14,1:47);
ar_8 = test_gamma(16,1:47);
ar_9 = test_gamma(18,1:47);
ar_10 = test_gamma(18,1:47);

% Average Iterations
ai_1 = test_gamma(3,1:47);
ai_2 = test_gamma(5,1:47);
ai_3 = test_gamma(7,1:47);
ai_4 = test_gamma(9,1:47);
ai_5 = test_gamma(11,1:47);
ai_6 = test_gamma(13,1:47);
ai_7 = test_gamma(15,1:47);
ai_8 = test_gamma(17,1:47);
ai_9 = test_gamma(19,1:47);
ai_10 = test_gamma(21,1:47);

%% Figure 1: Average Reward
f1 = figure;
hold on;
title('Q-learning 5 rooms test on gamma: Average Reward');
plot(x,ar_1,'--');
plot(x,ar_2,'-');
plot(x,ar_3,'-.');
plot(x,ar_4,':');
plot(x,ar_5,'--');
plot(x,ar_6,'-');
plot(x,ar_7,'-.');
plot(x,ar_8,':');
plot(x,ar_9,'--');
plot(x,ar_10,'-');
xticks([0 1 2 3 4 5 7 9 11 13 15 17 20 23 26 28 31 35 39 43 47]);
xticklabels([0 1 2 3 4 5 7 9 11 13 15 17 20 35 50 70 100 200 400 700 1000]);
legend({'$\gamma = 0.99$',
    '$\gamma = 0.975$',
    '$\gamma = 0.95$',
    '$\gamma = 0.925$',
    '$\gamma = 0.9$',
    '$\gamma = 0.85$',
    '$\gamma = 0.8$',
    '$\gamma = 0.7$',
    '$\gamma = 0.6$',
    '$\gamma = 0.5$'},'Location','southeast','Interpreter','latex');
xlim([0 47]);
xlabel('Number of episodes');
ylabel('Average reward');

%% Figure 2: Average Number of iterations pr. episode
f1 = figure;
hold on;
title('Q-learning 5 rooms test on gamma: Average Iterations');
plot(x,ai_1,'--');
plot(x,ai_2,'-');
plot(x,ai_3,'-.');
plot(x,ai_4,':');
plot(x,ai_5,'--');
plot(x,ai_6,'-');
plot(x,ai_7,'-.');
plot(x,ai_8,':');
plot(x,ai_9,'--');
plot(x,ai_10,'-');
xticks([0 1 2 3 4 5 7 9 11 13 15 17 20 23 26 28 31 35 39 43 47]);
xticklabels([0 1 2 3 4 5 7 9 11 13 15 17 20 35 50 70 100 200 400 700 1000]);
legend({'$\gamma = 0.99$',
    '$\gamma = 0.975$',
    '$\gamma = 0.95$',
    '$\gamma = 0.925$',
    '$\gamma = 0.9$',
    '$\gamma = 0.85$',
    '$\gamma = 0.8$',
    '$\gamma = 0.7$',
    '$\gamma = 0.6$',
    '$\gamma = 0.5$'},'Location','northeast','Interpreter','latex');
xlim([0 47]);
xlabel('Number of episodes');
ylabel('Average number of iterations pr. episode');

%% Figure 3: Average Number of iterations pr. episode
f1 = figure;
hold on;
title('Q-learning 5 rooms test on gamma: Reward / Iteration');
plot(x,ar_1./ai_1,'--');
plot(x,ar_2./ai_2,'-');
plot(x,ar_3./ai_3,'-.');
plot(x,ar_4./ai_4,':');
plot(x,ar_5./ai_5,'--');
plot(x,ar_6./ai_6,'-');
plot(x,ar_7./ai_7,'-.');
plot(x,ar_8./ai_8,':');
plot(x,ar_9./ai_9,'--');
plot(x,ar_10./ai_10,'-');
xticks([0 1 2 3 4 5 7 9 11 13 15 17 20 23 26 28 31 35 39 43 47]);
xticklabels([0 1 2 3 4 5 7 9 11 13 15 17 20 35 50 70 100 200 400 700 1000]);
legend({'$\gamma = 0.99$',
    '$\gamma = 0.975$',
    '$\gamma = 0.95$',
    '$\gamma = 0.925$',
    '$\gamma = 0.9$',
    '$\gamma = 0.85$',
    '$\gamma = 0.8$',
    '$\gamma = 0.7$',
    '$\gamma = 0.6$',
    '$\gamma = 0.5$'},'Location','northwest','Interpreter','latex');
xlim([0 47]);
xlabel('Number of episodes');
ylabel('Average reward pr. average number of iterations');