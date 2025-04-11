import random
from itertools import product


class SortingNetworkEnv:
    def __init__(self, n_wires, max_comparators):
        self.n_wires = n_wires
        self.max_comparators = max_comparators
        self.current_comparators = []
        self.all_possible_inputs = list(product([0, 1], repeat=n_wires))

    def reset(self):
        self.current_comparators = []
        return self._get_state_representation()
    
    def simplify_network(self):
        i = 0

        while i < len(self.current_comparators):
            comp = self.current_comparators[i]
            a, b = comp
            ok = False
            while not ok:
                found = False
                j = i + 1
                while j < len(self.current_comparators):
                    comp2 = self.current_comparators[j]
                    if comp2 == comp:
                        intervening = self.current_comparators[i+1:j]
                        if all(a not in c and b not in c for c in intervening):
                            self.current_comparators.pop(j)
                            found = True
                            break
                    j += 1
                if found:
                    ok = False
                else:
                    ok = True
            i += 1


    def step(self, action, best_network_size):
        i, j = action
        if i > j:
            i, j = j, i

        self.current_comparators.append((i, j))
        score = self._check_sorting_network(return_score=True)
        is_fully_correct = (score == 1.0)

        reward = 0
        done = False

        if is_fully_correct:
            self.simplify_network()

            current_size = len(self.current_comparators)
            
            if current_size < best_network_size:
                reward += 1000
            else:
                reward += 100

            done = True

        else:
            reward += 100 * score - 1
            
        if len(self.current_comparators) >= self.max_comparators * 2:
            reward -= 100
            done = True

        return self._get_state_representation(), reward, done

    def _get_state_representation(self):
        return tuple(self.current_comparators)

    def _check_sorting_network(self, return_score=False):
        passed = 0
        for input_values in self.all_possible_inputs:
            values = list(input_values)
            for i, j in self.current_comparators:
                if values[i] > values[j]:
                    values[i], values[j] = values[j], values[i]
            if all(values[k] <= values[k+1] for k in range(len(values) - 1)):
                passed += 1

        if return_score:
            return passed / len(self.all_possible_inputs)
        else:
            return passed == len(self.all_possible_inputs)


    def get_valid_actions(self):
        return [(i, j) for i in range(self.n_wires) for j in range(i + 1, self.n_wires)]


class QLearningAgent:
    def __init__(self, n_wires, max_comparators, learning_rate=0.1, discount_factor=0.995, exploration_rate=0.95):
        self.env = SortingNetworkEnv(n_wires, max_comparators)
        self.q_table = {}
        self.learning_rate = learning_rate
        self.discount_factor = discount_factor
        self.exploration_rate = exploration_rate
        self.valid_actions = self.env.get_valid_actions()

    def get_q_value(self, state, action):
        if (state, action) not in self.q_table:
            self.q_table[(state, action)] = 0.0
        return self.q_table[(state, action)]

    def choose_action(self, state):
        if random.uniform(0, 1) < self.exploration_rate:
            return random.choice(self.valid_actions)
        else:
            q_values = [self.get_q_value(state, action) for action in self.valid_actions]
            max_q = max(q_values)
            actions_with_max_q = [action for action, q in zip(self.valid_actions, q_values) if q == max_q]
            return random.choice(actions_with_max_q)

    def update_q_value(self, state, action, reward, next_state):
        best_next_action = max(self.valid_actions, key=lambda a: self.get_q_value(next_state, a))
        best_next_q_value = self.get_q_value(next_state, best_next_action)

        current_q = self.get_q_value(state, action)
        new_q = current_q + self.learning_rate * (reward + self.discount_factor * best_next_q_value - current_q)
        self.q_table[(state, action)] = new_q

    def train(self, n_episodes=1000):
        best_network = None
        best_network_size = float('inf')

        for episode in range(n_episodes):
            state = self.env.reset()
            done = False

            while not done:
                action = self.choose_action(state)
                next_state, reward, done = self.env.step(action, best_network_size)
                self.update_q_value(state, action, reward, next_state)
                state = next_state

            if reward > 0 and len(self.env.current_comparators) < best_network_size:
                best_network = self.env.current_comparators.copy()
                best_network_size = len(best_network)
                print(f"episode {episode}: found a better sorting network with {best_network_size} comparators")

            self.exploration_rate = max(0.01, self.exploration_rate * 0.99995)

            if episode % 100 == 0:
                print(f"episode {episode}, the best network: {best_network_size} comparators, er: {self.exploration_rate}")

        return best_network, best_network_size


def find_min_comparators_for_sorting_network(n_wires, max_episodes=100000):
    max_comparators = n_wires * (n_wires - 1) // 2

    agent = QLearningAgent(n_wires, max_comparators)
    best_network, best_size = agent.train(n_episodes=max_episodes)

    return best_network, best_size 


if __name__ == "__main__":
    n_wires = 6
    best_network, min_comparators = find_min_comparators_for_sorting_network(n_wires)

    print(f"\nfinal result for {n_wires} wires:")
    print(f"minimum numbers of comparators: {min_comparators}")
    print(f"network configuration: {best_network}")