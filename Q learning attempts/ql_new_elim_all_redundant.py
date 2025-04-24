import random
from itertools import product

class LayeredSortingNetworkEnv:
    def __init__(self, n_wires, max_comparators):
        self.n_wires = n_wires
        self.max_comparators = max_comparators
        self.current_layers = [[]]
        self.all_possible_inputs = list(product([0, 1], repeat=n_wires))
        self.depth = 0

    def reset(self):
        self.current_layers = [[]]
        self.depth = 0
        self.all_possible_inputs = list(product([0, 1], repeat=self.n_wires))
        return self._get_state_representation()
    
    def comparator_has_effect(self, i, j):
        for values in self.all_possible_inputs:
            if values[i] > values[j]:
                return True
        return False


    def step(self, action, best_network_size):
        if action == "NEXT_LAYER":
            if sum(len(layer) for layer in self.current_layers) < self.max_comparators:
                self.current_layers.append([])
                self.current_layers[self.depth] = sorted(self.current_layers[self.depth])
                self.depth += 1
                return self._get_state_representation(), -1, False
            else:
                return self._get_state_representation(), -10, True

        i, j = action
        if i > j:
            i, j = j, i

        is_redundant = not self.comparator_has_effect(i, j)
        self.current_layers[self.depth].append(((i, j), is_redundant))

        if is_redundant:
            return self._get_state_representation(), -5, False

        partial_score = self._check_sorting_network(i, j, return_score=True)
        is_valid = partial_score == 1.0
        total_comparators = sum(len(layer) for layer in self.current_layers)

        reward = 0
        done = False

        if is_valid:
            self.remove_redundant_comparators()
            reward = 10 * max_comparators / total_comparators
            done = True
        else:
            reward = 5 * partial_score * max_comparators / total_comparators

        if total_comparators > max_comparators:
            reward -= 10
            done = True

        return self._get_state_representation(), reward, done

    def _get_state_representation(self):
        state_layers = tuple(tuple(comp[0] for comp in layer) for layer in self.current_layers)
        return state_layers, self.depth


    def _check_sorting_network(self, i, j, return_score=False):
        passed = 0

        for idx, input_values in enumerate(self.all_possible_inputs):
            values = list(input_values)
            if values[i] > values[j]:
                values[i], values[j] = values[j], values[i]

            self.all_possible_inputs[idx] = tuple(values)
            if all(values[k] <= values[k + 1] for k in range(len(values) - 1)):
                passed += 1

        score = passed / len(self.all_possible_inputs)

        if return_score:
            return score
        else:
            return score == 1.0


    def get_valid_actions(self):
        used = {x for comp in self.current_layers[self.depth] for x in comp[0]}
        actions = [
            (i, j) for i in range(self.n_wires) for j in range(i + 1, self.n_wires)
            if i not in used and j not in used
        ]
        return actions
    
    def remove_redundant_comparators(self):
        new_layers = []
        for layer in self.current_layers:
            cleaned = [(i, j) for ((i, j), is_redundant) in layer if not is_redundant]
            if cleaned:
                new_layers.append([((i, j), False) for (i, j) in cleaned])
        self.current_layers = new_layers
        self.depth = len(self.current_layers) - 1 if self.current_layers else 0


    def simplify_network(self):
        i = 0
        while i < len(self.current_layers):
            layer = self.current_layers[i]
            j = 0

            while j < len(layer):
                comp = layer[j]
                a, b = comp
                ok = False

                while not ok:
                    found = False
                    k = i + 1

                    while k < len(self.current_layers):
                        next_layer = self.current_layers[k]
                        for comp2 in next_layer:
                            if a in comp2 or b in comp2:
                                if comp2 == comp:
                                    intervening = []
                                    for l in range(i + 1, k):
                                        intervening.extend(self.current_layers[l])

                                    if not any(a in c or b in c for c in intervening):
                                        next_layer.remove(comp2)
                                        found = True
                                        break
                        
                        if found:
                            ok = False
                            break

                        k += 1

                    if not found:
                        ok = True

                j += 1
            if i < len(self.current_layers) and len(self.current_layers[i]) == 0:
                self.current_layers.pop(i)
                self.depth -= 1
            elif i == len(self.current_layers):
                self.depth -= 1
            else:
                i += 1


class QLearningAgent:
    def __init__(self, n_wires, max_comparators, learning_rate=0.01, discount_factor=0.99, exploration_rate=1.0):
        self.env = LayeredSortingNetworkEnv(n_wires, max_comparators)
        self.q_table = {}
        self.learning_rate = learning_rate
        self.discount_factor = discount_factor
        self.exploration_rate = exploration_rate
        self.valid_actions = self.env.get_valid_actions()
        self.n_wires = n_wires
        self.max_comparators = max_comparators

    def get_q_value(self, state, action):
        if (state, action) not in self.q_table:
            self.q_table[(state, action)] = 0.0
        return self.q_table[(state, action)]

    def choose_action(self, state):
        valid_actions = self.env.get_valid_actions()
        if not valid_actions:  
            return "NEXT_LAYER"

        if random.uniform(0, 1) < self.exploration_rate:
            return random.choice(valid_actions)
        else:
            q_values = [self.get_q_value(state, a) for a in valid_actions]
            max_q = max(q_values)
            best_actions = [a for a, q in zip(valid_actions, q_values) if q == max_q]
            return random.choice(best_actions)


    def update_q_value(self, state, action, reward, next_state):
        next_valid_actions = self.env.get_valid_actions()
        max_next_q = (
            max(self.get_q_value(next_state, a) for a in next_valid_actions)
            if next_valid_actions else 0.0
        )

        current_q = self.get_q_value(state, action)
        new_q = current_q + self.learning_rate * (reward + self.discount_factor * max_next_q - current_q)
        self.q_table[(state, action)] = new_q


    def train(self, n_episodes=1000):
        best_network = None
        best_size = float('inf')

        for episode in range(n_episodes):
            state = self.env.reset()
            done = False

            while not done:
                action = self.choose_action(state)
                next_state, reward, done = self.env.step(action, best_size)
                self.update_q_value(state, action, reward, next_state)
                state = next_state

            total_comparators = sum(len(layer) for layer in self.env.current_layers)
            if reward > 0 and total_comparators < best_size:
                best_size = total_comparators
                best_network = [[comp[0] for comp in layer.copy()] for layer in self.env.current_layers]
                print(f"episode {episode}: found new best with {best_size} comparators and {len(best_network)} layers")

            self.exploration_rate = max(0.01, self.exploration_rate * 0.9995)
            if episode % 100 == 0:
                print(f"episode {episode}, best: {best_size}, exploration rate: {self.exploration_rate:.4f}")

        return best_network, best_size

def plot_network(n, layers):
    import matplotlib.pyplot as plt

    fig, ax = plt.subplots(figsize=(12, 0.6 * n))

    depth_spacing = 0.5
    micro_offset = 0.15
    wire_labels_padding = 0.15
    wire_start_offset = 0.5
    initial_layer_offset = 0.25
    wire_end_margin = 0.25

    comparator_draws = []

    x_pos = wire_start_offset + initial_layer_offset
    for depth in sorted(layers.keys()):
        comps = sorted(layers[depth], key=lambda x: (x[0], x[1]))
        placed = []
        max_offset = 0

        for a, b in comps:
            offset = 0
            while any((min(a, b) <= max(c, d) and max(a, b) >= min(c, d)) and offset == used_offset
                      for (c, d, used_offset) in placed):
                offset += 1
            placed.append((a, b, offset))
            max_offset = max(max_offset, offset)

        for a, b, offset in placed:
            y1 = n - 1 - a
            y2 = n - 1 - b
            x = x_pos + offset * micro_offset
            comparator_draws.append((x, y1, y2))

        x_pos += depth_spacing + max_offset * micro_offset

    final_x_pos = x_pos

    for i in range(n):
        y = n - 1 - i
        ax.plot([wire_start_offset, final_x_pos + wire_end_margin], [y, y], color='black', linewidth=1)
        ax.text(wire_start_offset - wire_labels_padding, y, f"{i}", va='center', ha='right', fontsize=10)

    for x, y1, y2 in comparator_draws:
        ax.plot([x, x], [y1, y2], color='red', linewidth=2)

    ax.set_xlim(0, final_x_pos + wire_end_margin + 0.5)
    ax.set_ylim(-1, n)
    ax.set_title(f'Odd-Even Merge Sort Network for n={n}\nComparators: {sum(len(c) for c in layers.values())}, Depth: {len(layers)}',
                 fontsize=14)
    ax.axis('off')
    plt.tight_layout()
    plt.show()

def find_layered_sorting_network(n_wires, max_comparators, episodes=10000):
    agent = QLearningAgent(n_wires, max_comparators)
    best_network, best_size = agent.train(n_episodes=episodes)
    return best_network, best_size

def convert_network_to_layers(network):
    return {depth: layer for depth, layer in enumerate(network)}

if __name__ == "__main__":
    n_wires = 10
    max_comparators = n_wires * (n_wires + 1)
    best_network, best_size = find_layered_sorting_network(n_wires, max_comparators, episodes=10000)
    
    layers = convert_network_to_layers(best_network)

    print(f"\nFinal result for {n_wires} wires:")
    print(f"Min comparators: {best_size}")
    print(f"Network structure (layers):")
    plot_network(n_wires, layers)