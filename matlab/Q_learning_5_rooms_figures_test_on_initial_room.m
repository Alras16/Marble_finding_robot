x = test_data(1,1:5);
data = test_data(2,1:5);

bar(x,data);
title('Q-learning 5 rooms test on initial room');
xticks([1 2 3 4 5]);
xticklabels({'Room 1','Room 2','Room 3','Room 4','Room 5'});
ylabel('Average Reward');