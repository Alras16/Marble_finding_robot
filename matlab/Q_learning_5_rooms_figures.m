x = q_learning_5_rooms_test_1(1,1:47);
episodes = q_learning_5_rooms_test_1(2,1:47);
success_rate = q_learning_5_rooms_test_1(3,1:47);
average_reward = q_learning_5_rooms_test_1(4,1:47);

yyaxis left
title('Q-learning 5 rooms test 2');
plot(x,success_rate*100);
xticks([0 1 2 3 4 5 7 9 11 13 15 17 20 23 26 28 31 35 39 43 47]);
xticklabels([0 1 2 3 4 5 7 9 11 13 15 17 20 35 50 70 100 200 400 700 1000]);
xlim([0 47]);
ylim([20 105]);
xlabel('Number of episodes');
ylabel('Success rate');

yyaxis right
plot(x,average_reward);
ylabel('Average reward');
ylim([-5500 250]);