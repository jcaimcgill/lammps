// unit tests for the LAMMPS base class

#include "library.h"
#include "lammps.h"
#include <mpi.h>
#include <cstdio>  // for stdin, stdout
#include <string>

#include "gtest/gtest.h"

TEST(lammps_open, null_args) {
    ::testing::internal::CaptureStdout();
    void *alt_ptr;
    void *handle = lammps_open(0,NULL, MPI_COMM_WORLD, NULL);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.substr(0,6).c_str(),"LAMMPS");
    int mpi_init=0;
    MPI_Initialized(&mpi_init);
    EXPECT_GT(mpi_init,0);
    LAMMPS_NS::LAMMPS *lmp = (LAMMPS_NS::LAMMPS *)handle;
    EXPECT_EQ(lmp->world, MPI_COMM_WORLD);
    EXPECT_EQ(lmp->infile, stdin);
    EXPECT_EQ(lmp->screen, stdout);
    EXPECT_NE(lmp->citeme, nullptr);
    ::testing::internal::CaptureStdout();
    lammps_close(handle);
    output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.substr(0,16).c_str(), "Total wall time:");
}

TEST(lammps_open, with_args) {
    const char *args[] = {"liblammps",
                          "-log", "none",
                          "-nocite"};
    char **argv = (char **)args;
    int argc = sizeof(args)/sizeof(char *);

    // MPI is already initialized
    MPI_Comm mycomm;
    MPI_Comm_split(MPI_COMM_WORLD, 0, 1, &mycomm);
    ::testing::internal::CaptureStdout();
    void *alt_ptr;
    void *handle = lammps_open(argc, argv, mycomm, &alt_ptr);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.substr(0,6).c_str(),"LAMMPS");
    EXPECT_EQ(handle,alt_ptr);
    LAMMPS_NS::LAMMPS *lmp = (LAMMPS_NS::LAMMPS *)handle;

    // MPI STUBS uses no real communicators
#if !defined(MPI_STUBS)
    EXPECT_NE(lmp->world, MPI_COMM_WORLD);
#endif

    EXPECT_EQ(lmp->world, mycomm);
    EXPECT_EQ(lmp->infile, stdin);
    EXPECT_EQ(lmp->logfile, nullptr);
    EXPECT_EQ(lmp->citeme, nullptr);
    ::testing::internal::CaptureStdout();
    lammps_close(handle);
    output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.substr(0,16).c_str(), "Total wall time:");
}

TEST(lammps_open_no_mpi, no_screen) {
    const char *args[] = {"liblammps",
                          "-log", "none",
                          "-screen", "none",
                          "-nocite"};
    char **argv = (char **)args;
    int argc = sizeof(args)/sizeof(char *);

    ::testing::internal::CaptureStdout();
    void *alt_ptr;
    void *handle = lammps_open_no_mpi(argc, argv, &alt_ptr);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(),"");
    EXPECT_EQ(handle,alt_ptr);
    LAMMPS_NS::LAMMPS *lmp = (LAMMPS_NS::LAMMPS *)handle;

    EXPECT_EQ(lmp->world, MPI_COMM_WORLD);
    EXPECT_EQ(lmp->infile, stdin);
    EXPECT_EQ(lmp->screen, nullptr);
    EXPECT_EQ(lmp->logfile, nullptr);
    EXPECT_EQ(lmp->citeme, nullptr);
    ::testing::internal::CaptureStdout();
    lammps_close(handle);
    output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "");
}

TEST(lammps_open_fortran, no_args) {

    // MPI is already initialized
    MPI_Comm mycomm;
    MPI_Comm_split(MPI_COMM_WORLD, 0, 1, &mycomm);
    int fcomm = MPI_Comm_c2f(mycomm);
    ::testing::internal::CaptureStdout();
    void *handle = lammps_open_fortran(0, NULL, fcomm, NULL);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.substr(0,6).c_str(),"LAMMPS");
    LAMMPS_NS::LAMMPS *lmp = (LAMMPS_NS::LAMMPS *)handle;

    // MPI STUBS uses no real communicators
#if !defined(MPI_STUBS)
    EXPECT_NE(lmp->world, MPI_COMM_WORLD);
#endif

    EXPECT_EQ(lmp->world, mycomm);
    EXPECT_EQ(lmp->infile, stdin);
    EXPECT_EQ(lmp->screen, stdout);
    EXPECT_NE(lmp->logfile, nullptr);
    EXPECT_NE(lmp->citeme, nullptr);
    ::testing::internal::CaptureStdout();
    lammps_close(handle);
    output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.substr(0,16).c_str(), "Total wall time:");
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
