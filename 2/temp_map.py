import matplotlib.pyplot as plt
import seaborn as sns

data = [
    [0, 2, 4, 7.8, 12.4, 17.4, 24.4, 32],
    [0, 3, 6.8, 11.8, 19.6, 26.2, 36.2, 48.2],
    [1, 3.8, 9.6, 16, 24.4, 34.6, 46.6, 62],
    [1, 4.6, 9.8, 17.4, 27.6, 36.4, 49.4, 64.2],
    [1, 5.2, 12.8, 20.8, 33.6, 41, 59.6, 84.2],
    [1, 4.6, 11.6, 21.6, 33.8, 46.6, 68, 93],
    [1, 5.4, 13, 24, 37, 50.2, 71, 99.6],
    [1, 5.8, 13.4, 24.8, 39.2, 56.2, 73.2, 97],
    [1.2, 6.2, 14.4, 27.2, 41.8, 57.4, 80.2, 105.6]
]
data.reverse()
h = sns.heatmap(data, xticklabels=[1000 + 1000 * x for x in range(8)],
                yticklabels=[10 - x for x in range(9)],
                cmap=sns.cm.rocket_r)
plt.xlabel('Tasks number')
plt.ylabel('Processors number')
plt.show()
