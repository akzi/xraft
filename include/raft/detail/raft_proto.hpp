#pragma once
namespace xraft
{
	namespace detail
	{
		struct vote_request
		{
			int64_t term_;
			std::string candidate_;
			int64_t last_log_index_;
			int64_t last_log_term_;
		};

		struct vote_response 
		{
			int64_t term_;
			bool vote_granted_;
		};

		struct log_entry
		{
			enum class type:char
			{
				e_append_log = 0,
				e_configuration
			};
			int64_t index_;
			int64_t term_;
			type type_;
			std::string log_data_;

			std::size_t bytes() const
			{
				return endec::get_sizeof(index_) +
					endec::get_sizeof(term_) +
					endec::get_sizeof(std::underlying_type<type>::type()) +
					endec::get_sizeof(log_data_);
			}
			std::string to_string() const
			{
				std::string buffer_;
				buffer_.resize(bytes());

				return buffer_;
			}
			void from_string(const std::string &buffer)
			{
				//todo decode buffer to log_entry
			}
		};

		struct append_entries_request
		{
			int64_t term_;
			std::string leader_id_;
			int64_t prev_log_index_;
			int64_t prev_log_term_;
			std::vector<log_entry>entries_;
			int64_t leader_commit_;
		};

		struct append_entries_response
		{
			int64_t term_;
			bool success_;
		};

		struct install_snapshot
		{
			int64_t term_;
			std::string leader_id_;
			int64_t last_included_index_;
			int64_t last_included_term_;
			int64_t offset_;
			std::string data_;
			bool done_;
		};

		struct install_snapshot_result 
		{
			int64_t term_;
		};
	}
}