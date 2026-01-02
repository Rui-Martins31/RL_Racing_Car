// Custom scripts
#include "control.hpp"

// Globals
#define DEBUG false
#define PATH_OUTPUT "output.csv"
#define EPISODE_MAX 1000

#define NN_TOPOLOGY {3, 3} // {3, 3, 3}
#define NN_LEARNING_RATE 0.005


// Auxiliary Functions
float velocity(float vel_x, float vel_y, float vel_z)
{
    return sqrt( pow(vel_x, 2) + pow(vel_y, 2) + pow(vel_z, 2) );
}

float remap(float value, float original_min, float original_max,
                         float new_min, float new_max)
{
    return new_min + (value - original_min) * (new_max - new_min) / (original_max - original_min);
}

int reward(bool out_of_bounds, float dist_raced)
{
    /*
    Rewards:
        - Out of bounds: -10
        - Complete lap: +10
        - Distance raced: +10 * distance
        - Fastest lap: +5
    */

    // Reward
    int reward_total = 0;

    if (out_of_bounds) reward_total += -10;
    reward_total += 10 * dist_raced;

    return reward_total;
}

// Agent
Agent::Agent(int agent_num)
    : id(agent_num), reward(0.0f), nn(NN_TOPOLOGY, true)
{
}

Agent::Agent(int agent_num, float agent_reward, const std::vector<Scalar>& weights)
    : id(agent_num), reward(agent_reward), nn(NN_TOPOLOGY, false)
{
    this->nn.setWeights(weights);
}


// Generation
Generation::Generation()
{
    // Pre-allocate
    this->arr_agents.reserve(this->AGENTS_NUM_TOTAL);

    // Load last generation
    if (!load_last_complete_generation(PATH_OUTPUT)) {

        // Start from scratch
        for (size_t i = 0; i < this->AGENTS_NUM_TOTAL; i++) {
            this->arr_agents.emplace_back(i);
        }

        this->generation_num_curr = 1;
        this->agent_num_curr      = 0;
    }
    else {
        // Add new agents
        this->populate();

        this->agent_num_curr = 0;
    }
}

bool Generation::load_last_complete_generation(const std::string& filepath)
{
    // Open CSV file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    // Check if file is empty
    file.seekg(0, std::ios::end);
    if (file.tellg() == 0) {
        file.close();
        return false;
    }
    file.seekg(0, std::ios::beg);

    // Parse all rows
    struct ParsedAgent {
        int generation;
        float reward;
        std::vector<Scalar> weights;
    };
    std::vector<ParsedAgent> all_agents;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string token;
        ParsedAgent agent;

        // Parse generation number
        if (!std::getline(ss, token, ',')) continue;
        agent.generation = std::stoi(token);

        // Parse reward
        if (!std::getline(ss, token, ',')) continue;
        agent.reward = std::stof(token);

        // Parse weights
        while (std::getline(ss, token, ',')) {
            agent.weights.push_back(std::stof(token));
        }

        all_agents.push_back(agent);
    }
    file.close();

    if (all_agents.empty()) {
        return false;
    }

    // Group by generation number
    std::map<int, std::vector<ParsedAgent>> generations;
    for (const auto& agent : all_agents) {
        generations[agent.generation].push_back(agent);
    }

    // Find complete generations and the last complete one
    int last_complete_gen = -1;
    std::vector<int> complete_gens;
    for (const auto& pair : generations) {
        if (static_cast<int>(pair.second.size()) == AGENTS_NUM_TOTAL) {
            complete_gens.push_back(pair.first);
            if (pair.first > last_complete_gen) {
                last_complete_gen = pair.first;
            }
        }
    }

    // No complete generation found
    if (last_complete_gen == -1) {
        // Clear the file
        std::ofstream clear_file(filepath, std::ios::trunc);
        clear_file.close();
        return false;
    }

    // Rewrite CSV keeping only complete generations
    std::ofstream out_file(filepath, std::ios::trunc);
    if (out_file.is_open()) {
        for (const auto& agent : all_agents) {
            // Check if this agent belongs to a complete generation
            bool is_complete = std::find(complete_gens.begin(), complete_gens.end(), agent.generation) != complete_gens.end();
            if (is_complete) {
                out_file << agent.generation << "," << agent.reward;
                for (const auto& w : agent.weights) {
                    out_file << "," << w;
                }
                out_file << "\n";
            }
        }
        out_file.close();
    }

    // Get agents from last complete generation and sort by reward (descending)
    std::vector<ParsedAgent>& last_gen_agents = generations[last_complete_gen];
    std::sort(last_gen_agents.begin(), last_gen_agents.end(),
        [](const ParsedAgent& a, const ParsedAgent& b) {
            return a.reward > b.reward;
        });

    // Keep top AGENTS_NUM_SURVIVE agents and add to arr_agents
    this->arr_agents.clear();
    int count = 0;
    for (const auto& agent : last_gen_agents) {
        if (count >= AGENTS_NUM_SURVIVE) break;
        this->arr_agents.emplace_back(count, agent.reward, agent.weights);
        count++;
    }

    // Set generation number
    this->generation_num_curr = last_complete_gen + 1;

    return true;
}

MessageClient Generation::step(int episode_cycles, MessageServer message)
{
    // Control struct
    MessageClient control;

    // Neural Network
    NeuralNetwork nn(
        NN_TOPOLOGY,
        true
    );

    // Inputs {vel, ang, pos}
    RowVector inputs(3);
    inputs[0] = velocity(message.speedX, message.speedY, message.speedZ), 0.0, 1.0;
    inputs[1] = message.angle;
    inputs[2] = message.trackPos;

    // Forward {accel, brake, steer}
    RowVector outputs = nn.propagateForward(inputs);

    // Output
    control.accel = remap(outputs[0], -1.0, 1.0, 0.0, 1.0);//outputs[0];
    control.brake = remap(outputs[1], -1.0, 1.0, 0.0, 1.0);//outputs[1];
    control.steer = outputs[2];
    
    
    // DO NOT CHANGE ----
    control.clutch = 0.0;
    control.focus  = 0.0;
    control.gear   = 1;
    
    // Check if car is out of track
    // or if it's the end of episode
    bool out_of_bounds  = (message.trackPos < -1.0 || message.trackPos > 1.0);
    bool end_of_episode = (episode_cycles % EPISODE_MAX == 0);
    if (out_of_bounds || end_of_episode) {
        control.meta = true;

        // Store weights
        int final_reward = reward(
            out_of_bounds,
            message.distRaced
        );
        this->arr_agents[this->agent_num_curr].nn.saveToCSV(
            PATH_OUTPUT,
            this->generation_num_curr,
            final_reward
        );

        // Update
        this->update((float)final_reward);
    }
    else control.meta = false;
    // DO NOT CHANGE ----

    // DEBUG 
    // std::cout << "Episode cycles: " << episode_cycles << std::endl;

    return control;
}

void Generation::update(float reward)
{
    // Update current agent
    this->arr_agents[this->agent_num_curr].reward = reward;

    // Check end of gen
    if (this->agent_num_curr == this->AGENTS_NUM_TOTAL - 1) {
        
        // DEBUG
        std::cout << "Starting new Generation number " << this->generation_num_curr+1 << std::endl;
        
        this->populate();
        this->generation_num_curr += 1;
        this->agent_num_curr       = 0;
    }
    else {
        this->agent_num_curr += 1;
    }

}

void Generation::populate()
{
    // Pick the top agents
    // In case there are more than the maximum
    std::cout << "Picking top agents..." << std::endl;
    if (this->arr_agents.size() > this->AGENTS_NUM_SURVIVE){

        // Sort agents by reward in descending order
        std::cout << "Sort agents..." << std::endl;
        std::sort(this->arr_agents.begin(), this->arr_agents.end(),
            [](const Agent& a, const Agent& b) {
                return a.reward > b.reward;
            });

        // Keep only the top agents
        std::cout << "Erase low reward agents..." << std::endl;
        this->arr_agents.erase(
            this->arr_agents.begin() + this->AGENTS_NUM_SURVIVE, 
            this->arr_agents.end()
        );
    }

    // Mutate and randomize new agents
    std::cout << "Giving birth to new agents..." << std::endl;
    for (size_t agent_idx = this->AGENTS_NUM_SURVIVE;
         agent_idx < this->AGENTS_NUM_TOTAL;
         agent_idx++)
    {
        srand(time(0));

        // New Agent
        float prob_new_agent = ((double)rand()) / RAND_MAX;
        if (prob_new_agent <= this->AGENT_PROB_NEW) {
            arr_agents.emplace_back(agent_idx);
            continue;
        }

        // Child
        std::vector<Scalar> weights_child;
        int father_1_idx  = (int)(((double)rand()) / RAND_MAX)*this->AGENTS_NUM_SURVIVE;
        int father_2_idx  = (int)(((double)rand()) / RAND_MAX)*this->AGENTS_NUM_SURVIVE;
        std::vector<Scalar> weights_father_1 
            = this->arr_agents[father_1_idx].nn.getWeights();
        std::vector<Scalar> weights_father_2
            = this->arr_agents[father_2_idx].nn.getWeights();

        for (size_t weight_idx = 0;
             weight_idx < this->arr_agents[agent_idx-1].nn.getWeightCount();
             weight_idx++)
        {
            // Select weight from father
            Scalar weight_temp;
            if (weight_idx % 2 == 0)
                weight_temp = weights_father_1[weight_idx];
            else
                weight_temp = weights_father_2[weight_idx];

            // Mutation
            float prob_mutation  = ((double)rand()) / RAND_MAX;
            if (prob_mutation <= this->MUTATION_PROB) {
                // FIX LATER SHOULDN'T JUST ADD BUT ALSO SUBTRACT
                weight_temp += this->MUTATION_CHANGE;
            }

            weights_child.emplace_back(weight_temp);
        }

        // Add child agent
        this->arr_agents.emplace_back(agent_idx, 0.0, weights_child);
        

    }
    

}
