import pandas as pd
import matplotlib.pyplot as plt


with open('dcurr.txt', 'r') as file:
    dcurr_data = file.readlines()

with open('k.txt', 'r') as file:
    k_data = file.readlines()

# Now, let's parse the data into a list of floats removing the string parts
dcurr_values = [float(line.strip().replace('dcurr: ', '')) for line in dcurr_data]
k_values = [float(line.strip().replace('k after: ', '')) for line in k_data]


# Convert lists to DataFrames
df_dcurr = pd.DataFrame(dcurr_values, columns=['dcurr'])
df_k = pd.DataFrame(k_values, columns=['k'])


# To make the plot less dense, we can sample the data
# Let's sample every 10th data point to make the plot less crowded

df_dcurr_sampled = df_dcurr.iloc[::20]  # Sample every 10th data point
df_k_sampled = df_k.iloc[::20]

# Adding a reference line at dcurr = 0.1818182497200514
stretch_rest_length = 0.1818182497200514
max_length = stretch_rest_length * 0.1 + stretch_rest_length

fig, ax1 = plt.subplots(figsize=(10, 6))  # Maintain the larger figure size

color = 'tab:red'
ax1.set_xlabel('Index')
ax1.set_ylabel('dcurr', color=color)
ax1.plot(df_dcurr_sampled['dcurr'], color=color)  # Thinner line
ax1.axhline(y=stretch_rest_length, color='green', linestyle='--', label=f'Rest length: {stretch_rest_length}')
ax1.axhline(y=max_length, color='red', linestyle='--', label=f'allowed max length: {max_length}')

ax1.tick_params(axis='y', labelcolor=color)
ax1.legend(loc='upper right')

ax2 = ax1.twinx()  # instantiate a second axes that shares the same x-axis
color = 'tab:blue'
ax2.set_ylabel('k', color=color)
ax2.plot(df_k_sampled['k'], color=color)  # Thinner line
ax2.tick_params(axis='y', labelcolor=color)

fig.tight_layout()  # otherwise the right y-label is slightly clipped
plt.title('Spring Length Variation with k Adjustment')
plt.show()

