import matplotlib.pyplot as plt

times = list(map(lambda x: x / 1000, [247469, 451862, 390433, 445767, 511697, 509975,
         369228, 473860, 392375, 376948]))
best = [17, 23, 22, 33, 31, 46, 43, 39, 44, 40]
diff = [51 - 17, 49 - 23, 55 - 22, 63 - 33, 62 - 31, 57 - 46, 65 - 43, 64 - 39,
        60 - 44, 59 - 40]

plt.plot(diff)
plt.xticks([i for i in range(10)])
plt.ylabel("Difference between worst and best solution")
plt.xlabel("i value")
plt.grid(True)
plt.show()

plt.plot(best)
plt.xticks([i for i in range(10)])
plt.ylabel("Best metric value")
plt.xlabel("i value")
plt.grid(True)
plt.show()

plt.plot(times)
plt.xticks([i for i in range(10)])
plt.ylabel("Execution time, s")
plt.xlabel("i value")
plt.grid(True)
plt.show()
