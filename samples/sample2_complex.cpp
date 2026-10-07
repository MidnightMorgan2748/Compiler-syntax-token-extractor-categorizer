// Sample 2: Advanced C++ Program with OOP, Templates, Exceptions, Memory
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

namespace AdvancedSystems {

    template <typename T>
    class IProcessor {
    public:
        virtual ~IProcessor() = default;
        virtual void process(const T& data) = 0;
        virtual auto getStatus() const noexcept -> bool = 0;
    };

    class NumericDataFilter : public IProcessor<int> {
    private:
        int threshold;
        mutable size_t processedCount;
        static inline size_t totalGlobalInstances = 0;

    protected:
        bool validate(int val) const {
            return val >= threshold;
        }

    public:
        explicit NumericDataFilter(int thresh) 
            : threshold(thresh), processedCount(0) {
            totalGlobalInstances++;
        }

        virtual ~NumericDataFilter() override {
            totalGlobalInstances--;
        }

        virtual void process(const int& data) override {
            if (data < 0) {
                throw std::invalid_argument("Negative values not permitted");
            }
            if (validate(data)) {
                processedCount++;
            }
        }

        virtual auto getStatus() const noexcept -> bool override {
            return processedCount > 0;
        }

        static size_t getGlobalInstances() {
            return totalGlobalInstances;
        }
    };
}

int main() {
    using namespace AdvancedSystems;

    try {
        std::unique_ptr<IProcessor<int>> filter = 
            std::make_unique<NumericDataFilter>(10);

        const int testValues[] = { 5, 12, 18, -3, 25 };

        for (int val : testValues) {
            try {
                filter->process(val);
            } catch (const std::invalid_argument& ex) {
                std::cerr << "Caught expected exception: " << ex.what() << "\n";
            }
        }

        if (filter->getStatus()) {
            std::cout << "Processing succeeded.\n";
        }
    } catch (...) {
        std::cerr << "Fatal unexpected failure.\n";
        return 1;
    }

    return 0;
}
