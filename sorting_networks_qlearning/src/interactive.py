import sys
from environment.simulation import SortingNetworkEnv
from environment.evaluation import evaluate_network


def interactive_test():
    try:
        n = int(input("Enter the number of wires (n): "))
    except ValueError:
        print("Enter a valid integer for the number of wires.")
        sys.exit(1)

    try:
        max_steps = int(input("Enter maximum number of steps: "))
    except ValueError:
        print("Enter a valid integer for the maximum number of steps.")
        sys.exit(1)

    env = SortingNetworkEnv(n, max_steps)
    state = env.reset()
    print(f"Initial state of the network: {state}")

    while True:
        print("\nAvailable actions:")
        print("1: Add comparators")
        print("2: Evaluate the network")
        print("3: Reset the network")
        print("4: Exit")
        choice = input("Choose a program (1-4): ").strip()

        if choice == "1":
            try:
                i = int(input("Enter first wire index (0 to n-1): "))
                j = int(input("Enter the second wire index (0 to n-1): "))
                action = ('add', (i, j))
                state, reward, done, _ = env.step(action)
                print(f"Current state : {state}")
                print(f"Reward: {reward}, Finished episode? {done}")
            except ValueError:
                print("Invalid input. Enter integer for the first wire index.")
            except Exception as e:
                print(f"Error: {e}")

        elif choice == "2":
            # evaluare rețea curenta folosind testul 0-1
            if evaluate_network(env.network, env.n):
                print("The network is a valid sorting network!")
            else:
                print("The network is not a valid sorting network yet!")

        elif choice == "3":
            state = env.reset()
            print("Network reset.")
            print(f"Initial state: {state}")

        elif choice == "4":
            print("Exiting interactive mode... Game over!")
            break
        else:
            print("Invalid option. Choose 1, 2, 3 or 4.")


if __name__ == "__main__":
    interactive_test()
