#include "Pch.hpp"
#include <Natrium/Main.hpp>

#include <Natrium/Template/Expected.hpp>

using namespace Na::Primitives;

static Na::Logger<> logger{ "Expected" };

enum class ErrorCode
{
    None = 0,
    InvalidInput, CalculationError, FileError
};

std::string_view error_to_string(ErrorCode ec)
{
    switch (ec)
    {
        case ErrorCode::InvalidInput: return "Invalid input";
        case ErrorCode::CalculationError: return "Calculation error";
        case ErrorCode::FileError: return "File error";
    }
    return "Unknown error";
}

Na::Expected<int, ErrorCode> parse_input(const std::string& str)
{
    try
    {
        return std::stoi(str);
    } catch (...)
    {
        return Na::Unexpected(ErrorCode::InvalidInput);
    }
}

Na::Expected<double, ErrorCode> calculate_sqrt(int val)
{
    if (val < 0)
    {
        return Na::Unexpected(ErrorCode::CalculationError);
    }

    return std::sqrt(val);
}

int main(int argc, char* argv[])
{
    if (auto expected = calculate_sqrt(16))
    {
        logger.printf(Na::Info, "Sqrt: {}", expected.value());
    } else
    {
        logger.printf(Na::Error, "Error calculating sqrt: {}", error_to_string(expected.error()));
    }
    
    if (auto expected = calculate_sqrt(-4))
    {
        logger.printf(Na::Info, "Sqrt: {}", expected.value());
    } else
    {
        logger.printf(Na::Error, "Error calculating sqrt: {}", error_to_string(expected.error()));
    }

    {
        auto expected = calculate_sqrt(-25);

        logger.printf(Na::Info, "Result: {}", expected.value_or(-1.0));
    }

    {
        auto expected =
            parse_input("16")
            .and_then(calculate_sqrt)       
		    .transform([](double d) -> std::string
            {
                return "Result: " + std::to_string(d);
            });

        if (expected)
        {
            logger.printf(Na::Info, "{}", expected.value());
        }
    }

    {
        auto expected =
            parse_input("-1")
            .and_then(calculate_sqrt) // fails
            .or_else([](ErrorCode ec) -> Na::Expected<double, ErrorCode>
            {
                if (ec == ErrorCode::CalculationError)
                {
				    logger.print(Na::Warn, "Recovering from CalculationError");
                    return 0.0; 
                }

			    return Na::Unexpected(ec);
            });

        if (expected)
        {
		    logger.printf(Na::Info, "Recovered value: {}", expected.value());
        }
    }

    return 0;
}