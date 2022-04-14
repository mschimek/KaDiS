#pragma once

#include <numeric>
#include <string>
#include <chrono>
#include <utility>
#include <vector>

#include <mpi.h>

template <class T>
class LocalMeasure {
  public:
    LocalMeasure() = delete;
    LocalMeasure(const std::string& name, MPI_Datatype mpi_vt, T neutral_el)
        : name_(name), mpi_vt_(mpi_vt), neutral_el_(neutral_el) {}

    void add(T el) { v_.push_back(el); }

    void reset() { v_.clear(); }

    template <class T1>
    struct Max {
        Max(const std::string name, std::vector<T1>&& max_els, const T1 total_max)
            : name_(name), max_els_(max_els), total_max_(total_max) {}

        std::string toString() {
            size_t id = 0;
            std::string str;
            for (auto& max_el : max_els_) {
                str += "\t" + name_ + "-" + std::to_string(id) + "=" +
                       std::to_string(max_el);
                id++;
            }
            str += "\t" + name_ + "=" + std::to_string(total_max_);
            return str;
        }
        void printOnRoot(MPI_Comm& comm) {
            int myrank = -1;
            MPI_Comm_rank(comm, &myrank);
            if (myrank == 0) {
                std::cout << toString();
            }
        }
        

      private:
        const std::string name_;
        const std::vector<T1> max_els_;
        const T1 total_max_;
    };

    LocalMeasure::Max<T> max(MPI_Comm& comm) {
        // Pad measurements
        size_t el_cnt = v_.size();
        size_t max_el_cnt = 0;
        MPI_Allreduce(&el_cnt, &max_el_cnt, 1, MPI_UNSIGNED_LONG, MPI_MAX, comm);
        while (v_.size() < max_el_cnt) {
            v_.push_back(neutral_el_);
        }

        // Aggregate over PEs
        std::vector<T> max_vec(v_.size(), 0);
        // Allreduce elements vector.
        MPI_Allreduce(v_.data(), max_vec.data(), v_.size(), mpi_vt_, MPI_MAX,
                      comm);
        reset();

        T total_max = std::accumulate(max_vec.begin(), max_vec.end(), neutral_el_);
        return Max<T>(name_, std::move(max_vec), total_max);
    }

  private:
    std::vector<T> v_;
    std::string name_;
    MPI_Datatype mpi_vt_;
    T neutral_el_;
};

template <class T>
class LocalDummyMeasure {
  public:
    LocalDummyMeasure() {}
    LocalDummyMeasure(const std::string&, MPI_Datatype, T) {}

    void add(T) {}

    void reset() {}

    struct Max {
        Max(const std::string, std::vector<T>&&) {}
        std::string toString() { return ""; }
        void printOnRoot(MPI_Comm& ) {}
    };

    LocalDummyMeasure::Max max(MPI_Comm& ) {
        std::vector<T> t;
        return Max("", std::move(t));
    }

  private:
};
