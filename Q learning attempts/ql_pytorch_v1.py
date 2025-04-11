import torch
import torch.nn as nn
import torch.optim as optim
import random
import numpy as np
from itertools import product
from collections import deque
import matplotlib.pyplot as plt

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

        self.current_layers[self.depth].append((i, j))
        partial_score = self._check_sorting_network(action, return_score=True)
        is_valid = partial_score == 1.0
        total_comparators = sum(len(layer) for layer in self.current_layers)

        reward = 0
        done = False

        if is_valid:
            self.simplify_network()
            reward = 100
            done = True
        else:
            reward = 10 * partial_score

        if total_comparators >= self.max_comparators * 2:
            reward -= 100
            done = True

        return self._get_state_representation(), reward, done

    def _get_state_representation(self):
        return tuple(tuple(layer) for layer in self.current_layers), self.depth

    def _check_sorting_network(self, action=None, return_score=False):
        passed = 0

        for idx, input_values in enumerate(self.all_possible_inputs):
            values = list(input_values)
            if action is not None:
                i, j = action
                if i > j:
                    i, j = j, i
                if values[i] > values[j]:
                    values[i], values[j] = values[j], values[i]
                self.all_possible_inputs[idx] = tuple(values)
            if all(values[k] <= values[k + 1] for k in range(len(values) - 1)):
                passed += 1

        score = passed / len(self.all_possible_inputs)
        return score if return_score else score == 1.0

    def get_valid_actions(self):
        used = {x for comp in self.current_layers[self.depth] for x in comp}
        actions = [(i, j) for i in range(self.n_wires) for j in range(i + 1, self.n_wires)
                   if i not in used and j not in used]
        actions.append("NEXT_LAYER")
        return actions

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


class DQN(nn.Module):
    def __init__(self, state_dim, action_dim, hidden_dim=128):
        super(DQN, self).__init__()
        self.model = nn.Sequential(
            nn.Linear(state_dim, hidden_dim),
            nn.ReLU(),
            nn.Linear(hidden_dim, action_dim)
        )

    def forward(self, x):
        return self.model(x)

class DQNSortingAgent:
    def __init__(self, n_wires, max_comparators, lr=0.0001, gamma=0.99, epsilon=0.9995, batch_size=64, buffer_size=10000):
        self.env = LayeredSortingNetworkEnv(n_wires, max_comparators)
        self.state_dim = n_wires * n_wires
        self.action_dim = (n_wires * (n_wires - 1)) // 2 + 1
        self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

        self.policy_net = DQN(self.state_dim, self.action_dim).to(self.device)
        self.target_net = DQN(self.state_dim, self.action_dim).to(self.device)
        self.target_net.load_state_dict(self.policy_net.state_dict())
        self.target_net.eval()

        self.optimizer = optim.Adam(self.policy_net.parameters(), lr=lr)
        self.loss_fn = nn.MSELoss()
        self.gamma = gamma
        self.epsilon = epsilon
        self.batch_size = batch_size
        self.buffer = deque(maxlen=buffer_size)
        self.action_index_map = self._generate_action_index_map(n_wires)

    def _generate_action_index_map(self, n_wires):
        actions = [(i, j) for i in range(n_wires) for j in range(i + 1, n_wires)]
        actions.append("NEXT_LAYER")
        return {a: idx for idx, a in enumerate(actions)}, actions

    def _state_to_tensor(self, state):
        flat = np.zeros(self.state_dim, dtype=np.float32)
        for depth, layer in enumerate(state[0]):
            for i, j in layer:
                flat[i * self.env.n_wires + j] = 1
        return torch.tensor(flat, dtype=torch.float32, device=self.device).unsqueeze(0)

    def choose_action(self, state):
        valid_actions = self.env.get_valid_actions()
        if random.random() < self.epsilon:
            return random.choice(valid_actions)

        state_tensor = self._state_to_tensor(state)
        q_values = self.policy_net(state_tensor).detach().cpu().numpy().flatten()
        idx_map, actions = self.action_index_map
        valid_idxs = [idx_map[a] for a in valid_actions]
        best_idx = max(valid_idxs, key=lambda i: q_values[i])
        return actions[best_idx]

    def remember(self, state, action, reward, next_state, done):
        self.buffer.append((state, action, reward, next_state, done))

    def replay(self):
        if len(self.buffer) < self.batch_size:
            return

        samples = random.sample(self.buffer, self.batch_size)
        idx_map, _ = self.action_index_map

        states = torch.cat([self._state_to_tensor(s) for s, _, _, _, _ in samples])
        next_states = torch.cat([self._state_to_tensor(ns) for _, _, _, ns, _ in samples])
        actions = torch.tensor([idx_map[a] for _, a, _, _, _ in samples], device=self.device)
        rewards = torch.tensor([r for _, _, r, _, _ in samples], dtype=torch.float32, device=self.device)
        dones = torch.tensor([d for _, _, _, _, d in samples], dtype=torch.bool, device=self.device)

        q_values = self.policy_net(states)
        next_q_values = self.target_net(next_states).detach()

        q_expected = q_values[range(self.batch_size), actions]
        q_target = rewards + self.gamma * next_q_values.max(1)[0] * (~dones)

        loss = self.loss_fn(q_expected, q_target)
        self.optimizer.zero_grad()
        loss.backward()
        self.optimizer.step()

    def train(self, episodes=1000):
        best_network = None
        best_size = float('inf')

        for episode in range(episodes):
            state = self.env.reset()
            done = False

            while not done:
                action = self.choose_action(state)
                next_state, reward, done = self.env.step(action, best_size)
                self.remember(state, action, reward, next_state, done)
                self.replay()
                state = next_state

            total_comparators = sum(len(layer) for layer in self.env.current_layers)
            if total_comparators < best_size and reward > 0:
                best_size = total_comparators
                best_network = [layer.copy() for layer in self.env.current_layers]
                print(f"Episode {episode}: New best with {best_size} comparators and {len(best_network)} layers")

            self.epsilon = max(0.01, self.epsilon * 0.995)

            if episode % 20 == 0:
                self.target_net.load_state_dict(self.policy_net.state_dict())
                print(f"Episode {episode}, ε = {self.epsilon:.3f}, best = {best_size}")

        return best_network, best_size

def plot_network(n, layers):
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


def convert_network_to_layers(network):
    return {depth: layer for depth, layer in enumerate(network)}


if __name__ == "__main__":
    n_wires = 6
    max_comparators = n_wires * (n_wires + 1)
    agent = DQNSortingAgent(n_wires, max_comparators)
    best_network, best_size = agent.train(episodes=1000)

    if best_network:
        layers = convert_network_to_layers(best_network)
        print(f"\nFinal result for {n_wires} wires:")
        print(f"Min comparators: {best_size}")
        plot_network(n_wires, layers)
    else:
        print("No valid sorting network found.")
