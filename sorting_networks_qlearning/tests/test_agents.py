import unittest
from src.environment.simulation import SortingNetworkEnv
from src.agents.qlearning_agent import QLearningAgent

class TestQLearningAgent(unittest.TestCase):
    def test_action_selection(self):
        env = SortingNetworkEnv(n=4)
        agent = QLearningAgent(env, epsilon=1.0)  # explorare totală
        state = env.reset()
        action = agent.select_action(state)
        self.assertIn(action, agent.action_space)

    def test_update(self):
        env = SortingNetworkEnv(n=4)
        agent = QLearningAgent(env, epsilon=0.0)  # exploatare
        state = env.reset()
        action = agent.select_action(state)
        # pentru test, simulam un "next_state" simplificat
        next_state = state + [action[1]]
        agent.update(state, action, reward=10, next_state=next_state, done=False)
        self.assertIn((tuple(state), action), agent.q_table)

if __name__ == '__main__':
    unittest.main()
