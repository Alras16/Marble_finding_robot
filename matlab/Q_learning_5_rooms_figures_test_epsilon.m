%% Data
x = test_epsilon(1,1:47);

% Average Reward
ar_1 = test_epsilon(2,1:47);
ar_2 = test_epsilon(4,1:47);
ar_3 = test_epsilon(6,1:47);
ar_4 = test_epsilon(8,1:47);
ar_5 = test_epsilon(10,1:47);
ar_6 = test_epsilon(12,1:47);
ar_7 = test_epsilon(14,1:47);
ar_8 = test_epsilon(16,1:47);
ar_9 = test_epsilon(18,1:47);
ar_10 = test_epsilon(18,1:47);

% Average Iterations
ai_1 = test_epsilon(3,1:47);
ai_2 = test_epsilon(5,1:47);
ai_3 = test_epsilon(7,1:47);
ai_4 = test_epsilon(9,1:47);
ai_5 = test_epsilon(11,1:47);
ai_6 = test_epsilon(13,1:47);
ai_7 = test_epsilon(15,1:47);
ai_8 = test_epsilon(17,1:47);
ai_9 = test_epsilon(19,1:47);
ai_10 = test_epsilon(21,1:47);

%% Figure 1: Average Reward
f1 = figure;
hold on;
title('Q-learning 5 rooms test on epsilon: Average Reward');
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
legend({'$\epsilon = 0.01$',
    '$\epsilon = 0.025$',
    '$\epsilon = 0.05$',
    '$\epsilon = 0.075$',
    '$\epsilon = 0.1$',
    '$\epsilon = 0.15$',
    '$\epsilon = 0.2$',
    '$\epsilon = 0.3$',
    '$\epsilon = 0.4$',
    '$\epsilon = 0.5$'},'Location','southeast','Interpreter','latex');
xlim([0 47]);
xlabel('Number of episodes');
ylabel('Average reward');

%% Figure 2: Average Number of iterations pr. episode
f1 = figure;
hold on;
title('Q-learning 5 rooms test on epsilon: Average Iterations');
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
legend({'$\epsilon = 0.01$',
    '$\epsilon = 0.025$',
    '$\epsilon = 0.05$',
    '$\epsilon = 0.075$',
    '$\epsilon = 0.1$',
    '$\epsilon = 0.15$',
    '$\epsilon = 0.2$',
    '$\epsilon = 0.3$',
    '$\epsilon = 0.4$',
    '$\epsilon = 0.5$'},'Location','northeast','Interpreter','latex');
xlim([0 47]);
xlabel('Number of episodes');
ylabel('Average number of iterations pr. episode');