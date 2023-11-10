#include <memory>
#include <vector>
#include <unordered_map>

class ISolution {
public:
    virtual ISolution* clone() const = 0;
    virtual double score () const = 0;
    virtual void mutate () = 0;
    virtual ~ISolution() {};
};

class Solution : public ISolution {
public:
    Solution(unsigned proc_num, const std::vector<unsigned>& works_len);
    double score () const override;
    void mutate () override;
    ISolution* clone() const override;
    std::unordered_map<unsigned, unsigned> get_timetable() const { return timetable_; }
protected:
    unsigned proc_num_;
    std::vector<unsigned> works_len_;
    std::unordered_map<unsigned, unsigned> timetable_;
};

class TestSolution : public Solution {
public:
    TestSolution(unsigned proc_num, const std::vector<unsigned>& works_len);
    ISolution* clone() const override;
    double score () const override;
};

class IMutator {
public:
    virtual void mutate (ISolution* sol) const = 0;
};

class Mutator : public IMutator {
public:
    Mutator(unsigned max_mutation_num);
    void mutate (ISolution* sol) const override;
private:
    unsigned max_mutation_num_;
};

class ITemperatureDecrease {
public:
    virtual double decrease(double current_temp, unsigned iter_num) const = 0;
};

class BoltzmannTemperatureDecrease : public ITemperatureDecrease {
public:
    double decrease(double current_temp, unsigned iter_num) const override;
};

class CauchyTemperatureDecrease : public ITemperatureDecrease {
public:
    double decrease(double current_temp, unsigned iter_num) const override;
};

class LogTemperatureDecrease : public ITemperatureDecrease {
public:
    double decrease(double current_temp, unsigned iter_num) const override;
};

class MainCycle {
public:
    MainCycle(const ISolution* start_sol, double start_temp, 
              const IMutator* mutator, const ITemperatureDecrease* temp_decrease);
    ISolution* process () const;
private:
    const ISolution* start_sol_;
    double start_temp_;
    const IMutator* mutator_;
    const ITemperatureDecrease* temp_decrease_;
};