/**
 * @file
 * @author Max Godefroy
 * @date 28/03/2021.
 */

#include "kryne-engine/UI/DearImGuiPerformanceMetrics.hpp"


float DearImGuiPerformanceMetrics::averageFrameTime(vector<float> times) const
{
    float time = 0.f;

    size_t i;
    for (i = 1; i < this->averageSize && i < times.size(); i++)
    {
        if (times[times.size() - i] == 0.)
            break;
        time += times[times.size() - i];
    }
    i--;

    return i ? time / i : 0.f;
}


float DearImGuiPerformanceMetrics::pushTime(const string &name, float value)
{
    auto it = this->timesBuffers.find(name);
    if (it == this->timesBuffers.end())
        it = this->timesBuffers.emplace(name, vector<float>(BUFFER_SIZE)).first;

    auto &times = it->second;
    times.erase(times.begin());
    times.push_back(value);
    return this->averageFrameTime(times);
}


void DearImGuiPerformanceMetrics::renderComponent(Process *process)
{
    if (ImGui::Begin("Performance metrics"))
    {
        auto data = process->getLastFrameTimeData();
        auto frameTime = this->pushTime("Frame times", data.frameTime.count());

        auto &frameTimes = this->timesBuffers.find("Frame times")->second;
        ImGui::PlotLines("Frame times", frameTimes.data(), frameTimes.size(),
                         0, nullptr,
                         0.f, FLT_MAX,
                         ImVec2(0, 50.f));
        ImGui::Text("Average frame time: %.2fms", frameTime * 1000);
        ImGui::Text("Average FPS: %.1f", frameTime == 0.f ? 0.f : 1. / frameTime);

        if (ImGui::BeginTable("TimingsTable", 2))
        {
            for (const auto &time : data.recordedTimes)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", time.first.c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2fms", this->pushTime(time.first, time.second.count()) * 1000);
            }
            ImGui::EndTable();
        }

        if (ImGui::TreeNode("Options"))
        {
            ImGui::DragInt("Averaging count", reinterpret_cast<int *>(&this->averageSize),
                           1.f, 1, BUFFER_SIZE);
            ImGui::TreePop();
        }
    }

    ImGui::End();
}
