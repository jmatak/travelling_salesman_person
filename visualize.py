import matplotlib.pyplot as plt
import sys

X, Y = [], []
solution = []
distance, duration, instance, algorithm = None, None, None, None
with open("./samples/" + sys.argv[1] + ".tsp") as f:
    lines = f.readlines()
    for l in lines:
        if not l: continue
        a = [float(i) for i in l.split()]
        X.append(a[0])
        Y.append(a[1])

with open("./logs/" + sys.argv[2] + ".tsp") as f:
    line = f.readline()
    solution = [int(i) for i in line.split(",")]

    distance = f.readline().strip()
    duration = f.readline().strip()
    instance = f.readline().strip()
    algorithm = f.readline().strip()


def draw_tour(tour):
    plt.figure()

    plt.scatter(X, Y)
    for i in range(len(X)):
        plt.text(X[i], Y[i], str(i))

    label = 'Distance:' + distance + "\n" + "Time: " + duration
    plt.plot([X[i] for i in tour] + [X[tour[0]]], [Y[i] for i in tour] + [Y[tour[0]]], c='g', label=label)
    plt.title("Instance: " + instance + " Algorithm: " + algorithm)
    plt.xticks([])
    plt.yticks([])
    plt.legend()


draw_tour(solution)
plt.show()
# plt.savefig("../logs/" + sys.argv[2] + ".png")

