from tensorflow.keras import layers, models

def build_simple_model(input_shape, output_size):
    print("26. models network_models.build_simple_model")
    model = models.Sequential()
    model.add(layers.Dense(64, activation='relu', input_shape=input_shape))
    model.add(layers.Dense(64, activation='relu'))
    model.add(layers.Dense(output_size, activation='linear'))
    model.compile(loss='mse', optimizer='adam')
    return model

